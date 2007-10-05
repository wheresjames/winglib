/*------------------------------------------------------------------
// net_session.h
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
// TNetSession
//
/// Implements a data connection session
/**

    The T_PROTOCOL class MUST implement
    
    - void RegisterFunctions( CDispatch *x_pDispatch );
        
        Called to register protocol specific functions

    - t_SOCKETEVENT GetEventHandle();

        Should return a waitable event handle or NULL.        

    - void ProcessEvents();

        Called when the event handle returned by GetEventHandle() is set.

    - void Destroy();

        Free all resources


    * If your T_PROTOCOL class inherits from CAutoSocket, these
      functions will already be provided.

  \code

    // Example of a more or less minimal protocol.
    class CEchoProtocol : 
            public CProtocol,
            public TBufferedPort< CAutoSocket >
    {
    public:

        virtual oexBOOL OnRead( oexINT x_nErr ) 
        {
            // Process the incomming data
            if ( !T_PORT::OnRead( x_nErr ) )
                return oexFALSE;

            // Just echo the data
            Send( Recv() );
            
            return oexFALSE; 
        }

        /// Closes the session when the socket closes
        virtual oexBOOL OnClose( oexINT x_nErr )
        {   CloseSession();
            return oexTRUE
        }

    };

  \endcode

*/
//==================================================================
template < typename T_PROTOCOL > 
    class TNetSession : 
        public os::CThread,
        public CDispatch
{
public:

	/// Default constructor
	TNetSession()
    {
        // Register functions
        m_cProtocol.OnRegisterFunctions( this );
    }

	/// Destructor
	virtual ~TNetSession()
    {   Stop();
    }

    /// Returns protocol reference
    T_PROTOCOL& Protocol()
    {   return m_cProtocol;
    }

public:

    /// Thread initialization
	virtual oexBOOL InitThread( oexPVOID x_pData ) 
    {
        return oexTRUE; 
    }

    /// Thread main loop
	virtual oexBOOL DoThread( oexPVOID x_pData ) 
    {
        // Ensure event handle
        if ( !GetCmdEvent() )
            return oexFALSE;

        // Get events
        os::CSys::t_WAITABLE phEvents[] = 
        {
            // 0 == Quit thread
            m_evQuit.GetHandle(),

            // 1 == Command waiting
            GetCmdEvent()->GetHandle(),

            // 2 == Port events
            m_cProtocol.GetEventHandle()
        };

        // How many valid handles
        oexUINT uHandles = oexSizeofArray( phEvents );

        // Don't look at the port handle if invalid
        if ( os::CEvent::vInvalid() == phEvents[ 2 ] )
            uHandles--;

        // Wait for something to happen
        oexINT nRet = os::CSys::WaitForMultipleObjects( uHandles, phEvents, oexFALSE, os::CSys::vInfinite() );

        // Time to quit?
        if ( !nRet )
            return oexFALSE;

        // Messages in the queue?
        else if ( 1 == nRet )
            ProcessQueue();

        // Port event
        else if ( 2 == nRet )
            m_cProtocol.ProcessEvents();

        return oexTRUE; 
    }

    /// Thread shutdown
	virtual oexINT EndThread( oexPVOID x_pData ) 
    {
        // Kill ther server
        m_cProtocol.Destroy();

        return 0; 
    }

private:

    /// Protocol class
    T_PROTOCOL              m_cProtocol;
};

