/*------------------------------------------------------------------
// worker_thread.h
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
// CWorkerThread
//
/// Thread locking mechanism
/**
	Encapsulates a Windows Mutex object to provide thread locking.	
*/
//==================================================================
class CWorkerThread : public CDispatch, public os::CThread
{
public:

	//==============================================================
	// CThread()
	//==============================================================
	/// Creates default lock object
	CWorkerThread() 
    { 
    }

	/// Destructor
	virtual ~CWorkerThread() 
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

        return oexTRUE; 
    }

	virtual oexBOOL DoThread( oexPVOID x_pData ) 
    {
        // Punt if no event handle
        if ( !GetCmdEvent() )
            return oexFALSE;

        // Get events
        os::CSys::t_WAITABLE phEvents[ 2 ] = 
        {
            // 0 == Quit thread
            m_evQuit.GetHandle(),

            // 1 == Command waiting
            GetCmdEvent()->GetHandle()

        };

        // How many valid handles
        oexUINT uHandles = 2;

        // Wait for something to happen
        oexINT nRet = os::CSys::WaitForMultipleObjects( uHandles, phEvents, oexFALSE, os::CSys::vInfinite() );

        // Time to quit?
        if ( !nRet )
            return oexFALSE;

        // Messages in the queue?
        else if ( 1 == nRet )
            ProcessQueue();

        return oexTRUE; 
    }

	virtual oexINT EndThread( oexPVOID x_pData ) 
    {
        return OnExit();
    }
	
private:

};

