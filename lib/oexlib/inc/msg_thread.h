/*------------------------------------------------------------------
// msg_thread.h
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted for commercial and 
// non-commercial purposes, provided that the following 
// conditions are met:
//
// * Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to 
//   endorse or promote products derived from this software without 
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#pragma once

//==================================================================
// CMsgThread
//
/// Provides threading support
/**
*/
//==================================================================
class CMsgThread : public CMsgObject, public os::CThread
{
public:

    /// Event handler list type
    typedef TAssoList< os::CSys::t_WAITABLE, CMsgAddress >    t_EventHandlerList;

public:

	/// Creates default lock object
	CMsgThread() 
    { 
        m_uTimeout = 0;
    }

	/// Destructor
	virtual ~CMsgThread() 
    { 
        Destroy();
    }

    /// Call to destoy the thread and release resources
    virtual void Destroy()
    {
        // Kill the thread
        CThread::Stop();
    }

    /// Called when the thread first starts
    /**
        If OnInit() returns a value less than zero,
        the thread will terminate.
    */
    virtual oexINT OnInit() { return 0; }

    /// Called continuously until the thread exits
    /*
        If OnIdle() returns a number greater than zero, OnIdle()
        will be called again even if there are no impending events.

        If OnIdle() returns zero, it will not be called again until
        an event occurs.

        If OnIdle() returns a number less than zero, then the thread
        will shutdown and exit.
    */
    virtual oexINT OnIdle() { return 0; }

    /// Called before the thread shuts down
    /**
        The return value is ignored
    */
    virtual oexINT OnExit() { return 0; }

private:

    // Os thread initialization
	virtual oexBOOL InitThread( oexPVOID x_pData ) 
    {
        // Do initialization
        if ( 0 > OnInit() )
            return oexFALSE;

        // Don't wait the first time
        m_uTimeout = 0;

        return oexTRUE; 
    }

	virtual oexBOOL DoThread( oexPVOID x_pData ) 
    {
        // Get events
        os::CSys::t_WAITABLE phEvents[ os::CSys::eMaximumWaitObjects ] = 
        {
            // 0 == Quit thread
            m_evQuit.GetHandle(),

            // 1 == Message waiting
            msgGetEvent().GetHandle(),

            // 2 == Configuration change was made
            m_evChange.GetHandle()
        };

        // How many valid handles
        oexUINT uHandles = 3;

        { // Add custom event handlers

            // Add extra events
            CTlLocalLock ll( m_lock );
            if ( ll.IsLocked() )
                for ( t_EventHandlerList::iterator it; 
                      os::CSys::vMaximumWaitObjects() > uHandles 
                      && m_lstEventHandlers.Next( it ); )
                    phEvents[ uHandles++ ] = it.Node()->key;

        } // end scope

        // Wait for something to happen
        oexINT nRet = os::CSys::WaitForMultipleObjects( uHandles, phEvents, oexFALSE, m_uTimeout );

        // Time to quit?
        if ( !nRet )
            return oexFALSE;

        // Messages in the queue?
        else if ( 1 == nRet )
            msgProcessQueue();

        // Config change?
        else if ( 2 == nRet )
            m_evChange.Reset();

        // Post event callback
        else if ( os::CSys::vMaximumWaitObjects() > (oexUINT)nRet )
        {
            // Get handler for this event
            t_EventHandlerList::iterator it = m_lstEventHandlers.Find( phEvents[ nRet ] );

            // Send the event message if we found a handler
            if ( it.IsValid() )
                msgOrb.Send( msgCreate( 0, it.Ptr() ) );

            // Reset event object
            os::CEvent::osResetEvent( (oex::os::CEvent::t_EVENT)phEvents[ nRet ] );

        } // end else if

        // Do idle processing
        m_uTimeout = OnIdle() ? 0 : os::CSys::vInfinite();

        return oexTRUE; 
    }

	virtual oexINT EndThread( oexPVOID x_pData ) 
    {
        return OnExit();
    }

    oexBOOL AddEventCallback( os::CSys::t_WAITABLE x_hEvent, CMsgAddress &x_Ma )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Add event handler
        m_lstEventHandlers[ x_hEvent ] = x_Ma;

        // Change things
        m_evChange.Set();

        return oexTRUE;
    }

    oexBOOL RemoveEventCallback( os::CSys::t_WAITABLE x_hEvent )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Remove event handler
        m_lstEventHandlers.Unset( x_hEvent );

        // Change things
        m_evChange.Set();

        return oexTRUE;
    }

	
private:

    /// Thread lock
    CTlLock                     m_lock;

    /// Wait timeout
    oexUINT                     m_uTimeout;

    /// Event handlers
    t_EventHandlerList          m_lstEventHandlers;

    /// Event triggered when an event handler is added or removed
    CTlEvent                    m_evChange;

};

