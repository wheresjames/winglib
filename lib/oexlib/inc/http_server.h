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
		eCleanupInterval = 60

	};

	/// Server callback
	typedef oexINT (*PFN_OnServerEvent)( oexPVOID x_pData, oexINT x_nEvent, oexINT x_nErr, THttpServer< T_PORT, T_SESSION > *x_pServer );

public:

	/// Holds information about a session
	class CSessionThread : public CThread
	{
	public:

		virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
		{
			// While thread is running and no transactions
			while ( GetStopEvent().Wait( 0 ) && !session.GetTransactions() && !port.IsError() )
			{
				// Process data if any
				if ( port.WaitEvent( oex::os::CIpSocket::eReadEvent, 100 ) )
					session.OnRead( 0 );

			} // end while

			// Drop the connection
//			port.Destroy();

			return oex::oexFALSE;
		}

		/// Session object
		T_SESSION		session;

		/// Session port
		T_PORT			port;
	};

	/// Session list type
	typedef TList< CSessionThread > t_LstSession;

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

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		// Wait for connect event
		if ( m_server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, 100 ) )
		{
			// Add a new session
			typename THttpServer::t_LstSession::iterator it = m_lstSessions.Append();

			// Attempt to connect session
			if ( !m_server.Accept( it->port )
				 || !it->port.WaitEvent( oex::os::CIpSocket::eConnectEvent ) )
			{
				m_lstSessions.Erase( it );

				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

			} // end if

			else
			{
				oexBOOL bAccept = oexTRUE;

				// Only accepting local connections?
				if ( m_bLocalOnly )
				{
					// Get local and remote address
					CStr8 sLocal = oexStrToMb( it->port.LocalAddress().GetDotAddress() );
					CStr8 sRemote = oexStrToMb( it->port.PeerAddress().GetDotAddress() );

					// Verify it is a local address
					if ( sLocal != sRemote && sRemote != "127.0.0.1" )
						bAccept = oexFALSE;

				} // end if

				if ( !bAccept )
				{
					m_lstSessions.Erase( it );

					if ( m_fnOnServerEvent )
						m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

				} // end if

				else
				{
					// Count a transaction
					m_nTransactions++;

					// Set the log file name
					it->session.SetLogFile( m_sLog.Ptr() );

					// Set the callback function for the data
					it->session.SetCallback( m_pSessionCallback, m_pSessionData );

					// Connect the port
					it->session.SetPort( &it->port );

					// Set default session timeout
					it->session.SetSessionTimeout( m_uSessionTimeout );

					// Let the session know the server id
					it->session.SetServerId( m_sServerId );

					// Enable sessions?
					if ( m_bEnableSessions )
						it->session.SetSessionObject( &m_pbSession, &m_lockSession );

					// Notify of new connection
					if ( m_fnOnServerEvent )
						m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

					// Start the thread
					it->Start();

				} // end if

			} // end else

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

		// Check for expired connections
		for ( typename t_LstSession::iterator it; m_lstSessions.Next( it ); )
			if ( !it->IsRunning() /* || !it->port.IsError() */ )
				it = m_lstSessions.Erase( it );

		// Is it time to cleanup?
		if ( m_uCleanup )
			m_uCleanup--;

		else
		{
//			oexEcho( "Cleaning up..." );
			m_uCleanup = eCleanupInterval * 10;

			// Attempt to cleanup session data
			oexAutoLock ll( m_lockSession );
			if ( ll.IsLocked() )
			{
				// Remove timed out sessions
				oexUINT ts = (oexUINT)oexGetUnixTime();
				for ( CPropertyBag8::iterator it; m_pbSession.List().Next( it ); )
					if ( !it->IsKey( "_ts" )
						 || ( it.Obj()[ "_ts" ].ToULong() + m_uSessionTimeout ) < ts )
					{
//						oexEcho( oexMks( oexT( "Erasing session : " ),
//										 ts, oexT( " : " ),
//										 m_uSessionTimeout, oexNL,
//										 it->PrintR() ).Ptr() );
						it = m_pbSession.List().Erase( it );

					} // end if

			} // end if

		} // end if

		return oexTRUE;
	}

	virtual oexINT EndThread( oex::oexPVOID x_pData )
	{
		// Stop the server
		m_server.Destroy();

		// Lose all sessions
		m_lstSessions.Destroy();

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
	{	return m_lstSessions.Size(); }

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

private:

	/// The TCP port to listen
	oexINT						m_nPort;

	/// Server port
	T_PORT						m_server;

	/// Transactions
	oexINT						m_nTransactions;

	/// List of session objects
	t_LstSession				m_lstSessions;

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

};
