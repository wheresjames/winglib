/*------------------------------------------------------------------
// port.h
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
// TNetServer
//
/// Implements a connection server
/**

  \code
  
    TNetServer< CIpPort, TNetSession< CMyProtocol > > server;

  \endcode

*/
//==================================================================
template < typename T_PORT, typename T_SESSION > 
    class TNetServer : 
        public os::CThread,
        public CDispatch,
        public T_PORT
{
public:

    /// Session list type
    typedef TAssoList< oexGUID, T_SESSION >     t_SessionList;

    /// Session type
    typedef typename t_SessionList::iterator    t_Session;

public:	

	/// Default constructor
	TNetServer()
    {
        // Register server functions
        T_PORT::RegisterFunctions( this );
    }

	/// Destructor
	virtual ~TNetServer()
    {   Destroy();
    }

    /// Destroys the network
    void Destroy()
    {
        Stop();
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
        // Get events
        os::CSys::t_WAITABLE phEvents[] = 
        {
            // 0 == Quit thread
            m_evQuit.GetHandle(),

            // 1 == Command waiting
            GetCmdEvent().GetHandle(),

            // 2 == Port events
            T_PORT::GetEventHandle()
        };

        // How many valid handles
        oexUINT uHandles = oexSizeofArray( phEvents );

        // Don't look at the port handle if invalid
        if ( !IsValidEventHandle() )
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
            T_PORT::OnPortEvent();

        return oexTRUE; 
    }

    /// Thread shutdown
	virtual oexINT EndThread( oexPVOID x_pData ) 
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );

        // Kill ther server
        T_PORT::Destroy();

        // Lose the session list
        m_sessions.Destroy();

        return 0; 
    }

    // Accept incomming connections
	virtual oexBOOL OnAccept( oexINT x_nErr )
    {
        // Accept the incomming connection
        T_PORT::Accept( AddSession( oexNULL ).Obj() );

        return oexTRUE;

    } // end else


public:

    /// Adds a session
    t_Session AddSession( oexGUID *x_pGuid )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return t_SessionList::iterator();

        // Create a unique id if none provided
        oexGUID guid;
        if ( !x_pGuid )
        {   os::CSys::CreateGuid( &guid );
            x_pGuid = &guid;
        } // end if

        // Return a session
        return m_sessions.Get( *x_pGuid );
    }

    /// Adds a session
    t_Session FindSession( oexGUID *x_pGuid )
    {
        // Ensure we have a valid guid
        if ( !x_pGuid || !*x_pGuid )
            return t_Session;

        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return t_SessionList::iterator();

        // Return a session
        return m_sessions.Find( *x_pGuid );
    }

private:

    /// Session lock
    CTlLock                         m_lockSession;

    /// Session list
    t_SessionList                   m_sessions;

};


//==================================================================
// TNetSession
//
/// Implements a data connection session
/**

  \code


  \endcode

*/
//==================================================================
template < typename T_PORT, typename T_PROTOCOL > 
    class TNetSession : 
        public os::CThread,
        public CDispatch,
        public T_PORT
{
public:

	/// Default constructor
	TNetSession()
    {
        // Register functions
        T_PORT::RegisterFunctions( this );
        m_cProtocol.RegisterFunctions( this );
    }

	/// Destructor
	virtual ~TNetSession()
    {   Stop();
    }

    /// Over-ride to provide message mapping
    virtual oexBOOL OnMap( oexCSTR x_pId, CStr x_sCmd, CReply &x_rReply )
    {
        // Check for local message
        if ( !x_pId || !*x_pId )
            return oexFALSE;

        // +++ Map to destination

        return oexTRUE; 
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
        // Get events
        os::CSys::t_WAITABLE phEvents[] = 
        {
            // 0 == Quit thread
            m_evQuit.GetHandle(),

            // 1 == Command waiting
            GetCmdEvent().GetHandle(),

            // 2 == Port events
            T_PORT::GetEventHandle()
        };

        // How many valid handles
        oexUINT uHandles = oexSizeofArray( phEvents );

        // Don't look at the port handle if invalid
        if ( !IsValidEventHandle() )
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
            T_PORT::OnPortEvent();

        return oexTRUE; 
    }

    /// Thread shutdown
	virtual oexINT EndThread( oexPVOID x_pData ) 
    {
        // Kill ther server
        T_PORT::Destroy();

        return 0; 
    }

private:

    /// Protocol class
    T_PROTOCOL              m_cProtocol;

};


/// Generic ip port
class CIpPort : public CAutoSocket
{
public:

    CIpPort()
    {
    }

    virtual ~CIpPort()
    {
        Destroy();
    }

    void RegisterFunctions( CDispatch *x_pDispatch )
    {
        // Sanity check
        if ( !x_pDispatch )
            return;

        // Register user callable functions
        x_pDispatch->OexRpcRegister( CAutoSocket, Connect );
        x_pDispatch->OexRpcRegister( CAutoSocket, Shutdown );
        x_pDispatch->OexRpcRegister( CAutoSocket, Bind );
        x_pDispatch->OexRpcRegister( CAutoSocket, Listen );
    }

    void OnPortEvent()
    {   OnSocketEvent(); 
    }

    oexBOOL IsValidEventHandle()
    {   return GetEventHandle() != os::CIpSocket::vInvalidEvent();
    }

	virtual oexBOOL OnRead( oexINT x_nErr ) 
    {


        return oexTRUE; 
    }

private:

};


class CNetProtocol
{
public:

    CNetProtocol()
    {
    }

    virtual ~CNetProtocol()
    {
    }

    void RegisterFunctions( CDispatch *x_pDispatch )
    {
        // Sanity check
        if ( !x_pDispatch )
            return;
    }

};

