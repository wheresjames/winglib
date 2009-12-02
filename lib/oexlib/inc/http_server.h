/*------------------------------------------------------------------
// http_server.h
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
// CHttpServer
//
/// Provides basic HTTP server
/**

  \code


  \endcode

*/
//==================================================================

template < typename T_PORT, typename T_SESSION > class THttpServer : public CThread
{
public:

	enum
	{
		/// Invalid event
		eSeNone = 0,

		/// Server connected
		eSeConnect = 1,

		/// Server disconnected
		eSeDisconnect = 2,

		/// Server accepted new connection
		eSeAccept = 3,

		/// Cleanup interval in seconds
		eCleanupInterval = 10,

		/// Maximum number of sessions to keep
		//  After this amount is reached, oldest sessions will be dropped
		eMaxSessions = 32

	};

	/// Server callback
	typedef oexINT (*PFN_OnServerEvent)( oexPVOID x_pData, oexINT x_nEvent, oexINT x_nErr, THttpServer< T_PORT, T_SESSION > *x_pServer );

public:

	/// Holds information about a session
	class CSessionInfo
	{
	public:

		/// Constructor
		CSessionInfo() { delete_me = oexFALSE; }

		/// Returns non-zero if we need to keep the connection running
		oex::oexBOOL IsValid()
		{
			// Does it need updating?
			if ( session.GetTransactions()
			     || port.IsError()
			     || ( !port.IsConnected() && !port.IsConnecting() )
			     || !port.IsActivity() )
				return oexFALSE;

			return oexTRUE;
		}

		// Updates the connection
		oex::oexBOOL Update( oexUINT x_uTimeout )
		{
			// Process data if any
			if ( port.WaitEvent( oex::os::CIpSocket::eReadEvent, x_uTimeout ) )
				session.OnRead( 0 );

			return oex::oexTRUE;
		}

		/// Session object
		T_SESSION		session;

		/// Session port
		T_PORT			port;

		/// Non-zero if this item is done
		oexBOOL			delete_me;
	};

	/// Holds information about a session
	class CSessionThread :
		public CSessionInfo,
		public CThread
	{
	public:

		virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
		{
			// While thread is running and no transactions
			while ( GetStopEvent().Wait( 0 ) && CSessionInfo::IsValid() )
				CSessionInfo::Update( 100 );

			return oex::oexFALSE;
		}

	};

	/// Session list type
	typedef TList< CSessionThread > t_LstSessionThread;

	/// Session list type
	typedef TList< CSessionInfo > t_LstSessionInfo;

	/// Holds information about a session
	class CSingleSessionThread : public CThread
	{
	public:

		/// Constructor
		CSingleSessionThread()
		{
			m_pSessionInfo = oexNULL;
			m_pSessionLock = oexNULL;
		}

		/// Do the work
		virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
		{
			if ( !m_pSessionInfo || !m_pSessionLock )
				return oexFALSE;

			// Lock the session list
			oexAutoLock ll( m_pSessionLock );
			if ( !ll.IsLocked() )
				return oexFALSE;

			// Update the sessions
			for ( typename t_LstSessionInfo::iterator it; m_pSessionInfo->Next( it ); )
				if ( it->IsValid() )
					it->Update( 0 );
				else
					it->delete_me = oexTRUE;

			return oex::oexTRUE;
		}

		/// Session information
		t_LstSessionInfo 	*m_pSessionInfo;

		/// Session information lock
		oexLock				*m_pSessionLock;

	};

public:

	/// Default constructor
	THttpServer()
	{
		m_pData = oexNULL;
		m_fnOnServerEvent = oexNULL;
		m_nTransactions = 0;
		m_pSessionData = oexNULL;
		m_pSessionCallback = oexNULL;
		m_bEnableSessions = oexFALSE;
		m_bLocalOnly = oexFALSE;
		m_uSessionTimeout = 60 * 60;
		m_uCleanup = 0;
		m_bMultiThreaded = oexTRUE;
	}

	~THttpServer()
	{
		// Stop the server
		Stop();

		m_pData = oexNULL;
		m_fnOnServerEvent = oexNULL;
		m_pSessionData = oexNULL;
		m_pSessionCallback = oexNULL;
		m_bEnableSessions = oexFALSE;
		m_bLocalOnly = oexFALSE;
	}

	oexBOOL StartServer( oexINT x_nPort, PFN_OnServerEvent fnOnServerEvent = oexNULL, oexPVOID x_pData = oexNULL )
	{
		Destroy();

		m_nPort = x_nPort;
		m_pData = x_pData;
		m_fnOnServerEvent = fnOnServerEvent;
		m_sServerId = oexStrToMb( oexGuidToString() );

		return 0 == CThread::Start();
	}

protected:

	oexBOOL ThreadStartServer()
	{
		// Bind to port
		if ( !m_server.Bind( m_nPort ) )
		{	if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeConnect, -1, this );
			return oexFALSE;
		} // end if

		// Listen
		if ( !m_server.Listen( 1 ) )
		{	if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeConnect, -2, this );
			return oexFALSE;
		} // end if

		// Notify that server is running
		if ( m_fnOnServerEvent )
			m_fnOnServerEvent( m_pData, eSeConnect, 0, this );

		return oexTRUE;
	}

	virtual oexBOOL InitThread( oex::oexPVOID x_pData )
	{
		// Attempt to start the server
		if ( !ThreadStartServer() )
			return oexFALSE;

		return oexTRUE;
	}

	/// Authenticate the connection
	virtual oexBOOL OnAuthenticate( T_PORT &port )
	{
		// Accepting anyone?
		if ( !m_bLocalOnly )
			return oexTRUE;

		// Get local and remote address
		CStr8 sLocal = oexStrToMb( port.LocalAddress().GetDotAddress() );
		CStr8 sRemote = oexStrToMb( port.PeerAddress().GetDotAddress() );

		// Verify it is a local address
		if ( sLocal != sRemote && sRemote != "127.0.0.1" )
			return oexFALSE;

		return oexTRUE;
	}

	oexBOOL InformSession( T_SESSION &session, T_PORT &port )
	{
		// Count a transaction
		session.SetTransactionId( m_nTransactions++ );

		// Set the log file name
		session.SetLogFile( m_sLog.Ptr() );

		// Set the callback function for the data
		session.SetCallback( m_pSessionCallback, m_pSessionData );

		// Connect the port
		session.SetPort( &port );

		// Set default session timeout
		session.SetSessionTimeout( m_uSessionTimeout );

		// Let the session know the server id
		session.SetServerId( m_sServerId );

		// Enable sessions?
		if ( m_bEnableSessions )
			session.SetSessionObject( &m_pbSession, &m_lockSession );

		return oexTRUE;
	}

	/// Accepts multi-threaded connection
	oexBOOL MultiAccept()
	{
		// Add a new session
		typename THttpServer::t_LstSessionThread::iterator it = m_lstSessionThread.Append();

		// Attempt to connect session
		if ( !m_server.Accept( it->port ) )
		{
			// Erase session
			m_lstSessionThread.Erase( it );

			// Let user in on the error
			if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

		} // end if

		else
		{
			// Authenticate the connection
			if ( !OnAuthenticate( it->port ) )
			{
				m_lstSessionThread.Erase( it );

				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

			} // end if

			else
			{
				// Fill in session information
				InformSession( it->session, it->port );

				// Notify of new connection
				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

				// Start the thread
				it->Start();

			} // end if

		} // end else

		return oexTRUE;
	}

	/// Accepts single-threaded connection
	oexBOOL SingleAccept()
	{
		// Must lock to make changes
		oexAutoLock ll( m_lockSessionInfo );
		if ( !ll.IsLocked() )
			return oexFALSE;

		// Add a new session
		typename THttpServer::t_LstSessionInfo::iterator it = m_lstSessionInfo.Append();

		// Attempt to connect session
		if ( !m_server.Accept( it->port ) )
		{
			// Erase session
			m_lstSessionInfo.Erase( it );

			// Let user in on the error
			if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

		} // end if

		else
		{
			// Authenticate the connection
			if ( !OnAuthenticate( it->port ) )
			{
				m_lstSessionInfo.Erase( it );

				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

			} // end if

			else
			{
				// Fill in session information
				InformSession( it->session, it->port );

				// Notify of new connection
				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

			} // end if

		} // end else

		// Start the processing thread if needed
		if ( !m_cSingleSessionThread.IsRunning() )
		{
			// Some info the thread will need
			m_cSingleSessionThread.m_pSessionInfo = &m_lstSessionInfo;
			m_cSingleSessionThread.m_pSessionLock = &m_lockSessionInfo;

			// Start the thread
			m_cSingleSessionThread.Start();

		} // end if

		return oexTRUE;
	}

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		// Wait for connect event
		if ( m_server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, 100 ) )
		{
			if ( m_bMultiThreaded )
				MultiAccept();
			else
				SingleAccept();

		} // end if

		// Did we lose the connection?
		else if ( m_server.IsError() )
		{
			// Drop old socket
			m_server.Destroy();

			// Wait a bit
			oexSleep( 3000 );

			// Restart the server
			ThreadStartServer();

		} // end else

		// Clean up expired connections
		CleanupConnections();

		// Is it time to cleanup sessions?
		if ( m_uCleanup )
			m_uCleanup--;
		else
			CleanupSessions();

		return oexTRUE;
	}

	oexBOOL CleanupConnections()
	{
		// Check for expired connections
		for ( typename t_LstSessionThread::iterator it; m_lstSessionThread.Next( it ); )
			if ( !it->IsRunning() )
				it = m_lstSessionThread.Erase( it );

		// Check for expired connections
		for ( typename t_LstSessionInfo::iterator it; m_lstSessionInfo.Next( it ); )
			if ( it->delete_me )
			{
				// Must lock to make changes
				oexAutoLock ll( m_lockSessionInfo );
				if ( !ll.IsLocked() )
					return oexFALSE;

				// Erase item
				it = m_lstSessionInfo.Erase( it );

			} // end if

		return oexTRUE;
	}

	oexBOOL CleanupSessions()
	{
//		oexEcho( "Cleaning up..." );
		m_uCleanup = eCleanupInterval * 10;

		// Attempt to cleanup session data
		oexAutoLock ll( m_lockSession );
		if ( ll.IsLocked() )
		{
			// +++ This gets us by, but it would be nice to drop the oldest
			//     connections based on _ts.  Currently just dropping anything
			//     that hasn't communicated in 30 seconds when we're over the limit.

			// Do we need to drop sessions?
			oexBOOL bDrop = eMaxSessions < m_pbSession.Size();

			// Remove timed out sessions
			oexUINT ts = (oexUINT)oexGetUnixTime();
			for ( CPropertyBag8::iterator it; m_pbSession.List().Next( it ); )
				if ( !it->IsKey( "_ts" )
					 || ( it.Obj()[ "_ts" ].ToULong() + m_uSessionTimeout ) < ts
					 || ( bDrop && ( it.Obj()[ "_ts" ].ToULong() + 30 ) < ts )
				   )
				{
//					oexEcho( oexMks( oexT( "Erasing session : " ),
//									 ts, oexT( " : " ),
//									 m_uSessionTimeout, oexNL,
//									 it->PrintR() ).Ptr() );
					it = m_pbSession.List().Erase( it );

				} // end if

		} // end if

		return oexTRUE;
	}

	virtual oexINT EndThread( oex::oexPVOID x_pData )
	{
		// Ensure the session thread has stopped
		m_cSingleSessionThread.Stop();

		// Stop the server
		m_server.Destroy();

		// Lose all sessions
		m_lstSessionThread.Destroy();

		// Lose session info objects
		m_lstSessionInfo.Destroy();

		// Notify that server is running
		if ( m_fnOnServerEvent )
			m_fnOnServerEvent( m_pData, eSeDisconnect, 0, this );

		return oexTRUE;
	}

public:

	/// Returns the number of client transactions serviced
	oexINT GetNumTransactions()
	{	return m_nTransactions; }

	/// Sets the session callback function
	void SetSessionCallback( oexPVOID x_pCallback, oexPVOID x_pData )
	{	m_pSessionCallback = x_pCallback; m_pSessionData = x_pData; }

	/// Returns the number of active sessions
	oexINT GetNumActiveClients()
	{	return m_lstSessionThread.Size() + m_lstSessionInfo.Size(); }

	/// Sets the log file name
	oexBOOL SetLogFile( oexCSTR x_pLog )
	{	m_sLog = x_pLog;
		return oexTRUE;
	}

	/// Returns reference to port object
	T_PORT& Port() { return m_server; }

	/// Enable / disable sessions
	void EnableSessions( oexBOOL b ) { m_bEnableSessions = b; }

	/// Enable / disable remote connections
	void EnableRemoteConnections( oexBOOL b ) { m_bLocalOnly = !b; }

	/// Sets the length of time that session data is to be valid
	void SetSessionTimeout( oexUINT uTo )
	{	m_uSessionTimeout = uTo; }

	/// Enable / disable multi threading
	//  For some reason, I enable doing this
	//  while the server is running ;)
	void EnableMultiThreading( oexBOOL b )
	{	m_bMultiThreaded = b; }

private:

	/// The TCP port to listen
	oexINT						m_nPort;

	/// Server port
	T_PORT						m_server;

	/// Transactions
	oexLONG						m_nTransactions;

	/// List of session thread objects (multi-threaded)
	t_LstSessionThread			m_lstSessionThread;

	/// List of session info objects (single-threaded)
	t_LstSessionInfo			m_lstSessionInfo;

	/// Runs single threaded sessions
	CSingleSessionThread		m_cSingleSessionThread;

	/// Data passed to m_fnOnServerEvent
	oexPVOID					m_pData;

	/// Function to call on server event
	PFN_OnServerEvent			m_fnOnServerEvent;

	/// Data passed to session callback
	oexPVOID					m_pSessionData;

	/// Pointer to session callback function
	oexPVOID					m_pSessionCallback;

	/// Log file name
	CStr						m_sLog;

	/// Enable sessions
	oexBOOL						m_bEnableSessions;

	/// Unique server id
	CStr8						m_sServerId;

	/// Stores session data
	CPropertyBag8				m_pbSession;

	/// Lock for session data access
	CLock						m_lockSession;

	/// Non-zero to accept local connections only
	oexBOOL						m_bLocalOnly;

	/// Length of time in seconds that session data is to be valid
	oexUINT						m_uSessionTimeout;

	/// Time to cleanup sessions;
	oexUINT						m_uCleanup;

	/// Non-zero to enable multi-threading
	oexBOOL						m_bMultiThreaded;

	/// Session info lock
	oexLock						m_lockSessionInfo;

};
