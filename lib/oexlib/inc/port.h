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


//==================================================================
// TNetServer
//
/// Implements a connection server
/**

  \code
  
    /// Example
    TNetServer< CAutoPort, CMyProtocol > server;

    // Start the server thread
    server.Start();

    // Interact with CAutoPort through the thread buffer, i.e. CDispatch
    server.Queue( oexCall( oexT( "Bind" ), 80 ) );
    CReply reply = server.Queue( oexCall( oexT( "Listen" ), 0 ) );

    // Wait for listen to complete
    if ( !reply.Wait( oexDEFAULT_TIMEOUT ).GetReply() )
        ; // Error

  \endcode

*/
//==================================================================
template < typename T_PORT, typename T_PROTOCOL > 
    class TNetServer : 
        public os::CThread,
        public CDispatch,
        public T_PORT
{
public:

    /// Session list type
    typedef TAssoList< oexGUID, TNetSession< T_PROTOCOL > >     t_SessionList;

    /// Session type
    typedef typename t_SessionList::iterator    t_Session;

public:	

	/// Default constructor
	TNetServer()
    {
        m_uTotalConnections = 0;

        // Register port functions
        T_PORT::RegisterFunctions( this );

        // Register server functions
        OexRpcRegister( TNetServer, CloseSession );
        OexRpcRegister( TNetServer, Reset );
    }

	/// Destructor
	virtual ~TNetServer()
    {   Destroy();
    }

    /// Destroys the network
    void Destroy()
    {
        os::CThread::Stop();

        m_uTotalConnections = 0;
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
            T_PORT::GetEventHandle()
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
            T_PORT::ProcessEvents();

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
        // Get a new session object
        t_Session session = AddSession( oexNULL );
        if ( !session.IsValid() )
            return oexFALSE;

        // Accept the incomming connection
        T_PORT::Accept( session.Obj().Protocol() );

        // Start the session thread
        session.Obj().Start();

        // Count a connection
        m_uTotalConnections++;

        return oexTRUE;

    } // end else

public:

    /// Adds a session
    t_Session AddSession( oexGUID *x_pGuid = oexNULL )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return t_Session();

        // Create a unique id if none provided
        oexGUID guid;
        if ( !x_pGuid )
        {   os::CSys::CreateGuid( &guid );
            x_pGuid = &guid;
        } // end if

        // Create the session
        t_Session session = m_sessions.Get( *x_pGuid );
        if ( !session.IsValid() )
            return session;

        // Set session information
        session.Obj().Protocol().SetServerDispatch( this );
        session.Obj().Protocol().SetSessionId( *x_pGuid );

        return session;
    }

    /// Adds a session
    CDispatch FindSession( oexCONST oexGUID &x_rGuid )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return CDispatch();

        t_Session session = m_sessions.Find( x_rGuid );
        if ( !session.IsValid() )
            return CDispatch;

        // Return a session
        return session.Obj();
    }

    /// Returns existing session or adds a new one if needed
    CDispatch GetSession( oexCONST oexGUID &x_rGuid )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return CDispatch();

        // Find existing session
        CDispatch session = FindSession( x_rGuid );
        if ( session.IsValid() )
            return session;

        // Just add a new one
        t_Session itSession = AddSession( &x_rGuid );
        if ( !itSession.IsValid() )
            return session;

        return itSession.Obj();
    }

    /// Returns the specified session by index
    CDispatch GetSession( oexUINT uIndex )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return CDispatch();

        // Get the session by index
        t_Session session = m_sessions.GetByIndex( uIndex );
        if ( !session.IsValid() )
            return CDispatch();

        // Return a session
        return session.Obj();
    }

    /// Adds a session
    oexBOOL CloseSession( oexCONST oexGUID &x_rGuid )
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Delete the session if it exists
        m_sessions.Unset( x_rGuid );

        return oexTRUE;
    }

    /// Closes the server and all sessions
    oexBOOL Reset()
    {
        // Lock the session list
        CTlLocalLock ll( m_lockSession );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Kill the server port
        T_PORT::Destroy();

        // Lose the session list
        m_sessions.Destroy();

        return oexTRUE;
    }

    /// Session list
    t_SessionList& GetSessionList()
    {   return m_sessions; }

    /// Lock for th session list
    operator CTlLock&() 
    {   return m_lockSession; }

    /// Returns the total number of connections since the server started
    oexUINT GetTotalConnections()
    {   return m_uTotalConnections; }

private:

    /// Session lock
    CTlLock                         m_lockSession;

    /// Session list
    t_SessionList                   m_sessions;

    /// Total number of connections the server has handled
    oexUINT                         m_uTotalConnections;

};


/// Basic port class
template < typename T_PORT, typename T_BUFFER = CCircBuf >
    class TBufferedPort : public T_PORT
{
public:

    /// Default constructor
    TBufferedPort()
    {
        m_bTxBlocking = oexFALSE;
    }

    /// Destructor
    virtual ~TBufferedPort()
    {

    }   

    // When data is read
	virtual oexBOOL OnRead( oexINT x_nErr )
    {
        // Process the incomming data
        if ( !T_PORT::OnRead( x_nErr ) )
	        return oexFALSE;

        // Buffer the data
        return Rx().Write( Recv() );
    }

    // When data is written
    virtual oexBOOL OnWrite( oexINT x_nErr )
    {
	    // Lock the transmit buffer
	    CTlLocalLock ll( Tx() );
	    if ( !ll.IsLocked() ) 
		    return oexFALSE;

	    // Not blocking now
	    m_bTxBlocking = oexFALSE;

	    // Send more data
	    OnTx( 0 );

	    return oexTRUE;
    }

    // When data is written
    virtual oexBOOL OnTx( oexINT x_nErr )
    {
	    // Lock the transmit buffer
	    CTlLocalLock ll( Tx() );
	    if ( !ll.IsLocked() ) 
		    return oexFALSE;

	    // Punt if we're blocking
	    if ( m_bTxBlocking ) 
            return oexTRUE;

	    oexUCHAR buf[ 1024 ];
	    oexUINT uReady = 0;

	    // Read blocks of data
	    while ( Tx().Peek( buf, sizeof( buf ), &uReady ) )
	    {
            // Send data
            oexINT res = Send( buf, uReady );

            // Returns zero if tx'er is full
            if ( !res )
            {
                m_bTxBlocking = oexTRUE;

                return oexFALSE;

            } // end if

		    // Remove the number of bytes sent from the buffer
		    else 
                Tx().AdvanceReadPtr( res );

	    } // end while

        return oexTRUE;
    }

public:

	//==============================================================
	// Write()
	//==============================================================
	/// Writes data to buffer
	/**
		\param [in] x_pData		-	Buffer containing write data
		\param [in] x_uSize		-	Size of the buffer in pData
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( oexCONST oexPVOID x_pData, oexUINT x_uSize )
    {
        // Buffer the data
        if ( !m_tx.Write( x_pData, x_uSize ) )
            return oexFALSE;

        // Start the tx'er
        OnTx( 0 );

        return oexTRUE;
    }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a NULL terminated string to buffer
	/**
		\param [in] x_pStr		-	Pointer to NULL terminated string
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( oexCSTR8 x_pStr )
    {	return Write( (oexPVOID)x_pStr, zstr::Length( x_pStr ) ); }


	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to buffer
	/**
		\param [in] x_sStr		-	String to be sent
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( CStr8 &x_sStr )
    {	return Write( (oexPVOID)x_sStr.Ptr(), x_sStr.Length() ); }

public:

    /// Returns a reference to the rx buffer
    T_BUFFER& Rx() 
    {   return m_rx; }

    /// Returns a reference to the tx buffer
    T_BUFFER& Tx() 
    {   return m_tx; }    

private:

    /// Non-zero if tx'er is blocking
    oexBOOL                     m_bTxBlocking;

    /// Rx buffer
    T_BUFFER                    m_rx;

    /// Tx buffer
    T_BUFFER                    m_tx;

};

/// Inherit from this class to implement a network protocol
class CProtocol
{
public:

    /// Default constructor
    CProtocol()
    {   m_pDispatch = oexNULL; 
        guid::ZeroGuid( &m_guidSessionId );
    }

    /// Returns a pointer to the server dispatch object
    CDispatch* Server()
    {   return m_pDispatch; }

    /// Set dispatch object
    void SetServerDispatch( CDispatch *pDispatch )
    {   m_pDispatch = pDispatch; }

    /// Sets the session id
    void SetSessionId( oexGUID &x_guid )
    {   guid::CopyGuid( &m_guidSessionId, &x_guid ); }

    /// Returns the session id
    oexGUID& GetSessionId()
    {   return m_guidSessionId; }

    /// Closes the session
    void CloseSession()
    {
        if ( m_pDispatch )
            m_pDispatch->Queue( 0, oexCall( oexT( "CloseSession" ), m_guidSessionId ) );
    }

    /// Over-ride to register interface functions
    virtual void OnRegisterFunctions( CDispatch *x_pDispatch )
    {
        if ( !x_pDispatch )
            return;

        x_pDispatch->OexRpcRegister( CProtocol, CloseSession );
    }

private:

    /// Session id
    oexGUID             m_guidSessionId;

    /// Pointer to server dispatch object
    CDispatch           *m_pDispatch;

};


/// Generic ip port
template < typename T_PORT >
    class TEchoProtocol : 
        public CProtocol,
        public T_PORT
{
public:

    virtual void OnRegisterFunctions( CDispatch *x_pDispatch )
    {
        // Call the base class
        CProtocol::OnRegisterFunctions( x_pDispatch );

        // Register interface functions
        T_PORT::RegisterFunctions( x_pDispatch );
    }

	virtual oexBOOL OnRead( oexINT x_nErr ) 
    {
        // Process the incomming data
        if ( !T_PORT::OnRead( x_nErr ) )
	        return oexFALSE;

        // Just echo the data
        Send( Recv() );
        
        return oexTRUE; 
    }

    /// Closes the session when the socket closes
	virtual oexBOOL OnClose( oexINT x_nErr )
    {
        CloseSession();

        return oexTRUE;
    }
};

