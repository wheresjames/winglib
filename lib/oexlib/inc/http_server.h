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
		eSeAccept = 3

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
			// Process the request
			while ( !session.GetTransactions()
					&& port.WaitEvent( oex::os::CIpSocket::eReadEvent ) )
				session.OnRead( 0 );

			// Drop the connection
			port.Destroy();

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
	}

	~THttpServer()
	{
		m_pData = oexNULL;
		m_fnOnServerEvent = oexNULL;
		m_pSessionData = oexNULL;
		m_pSessionCallback = oexNULL;
	}

	oexBOOL StartServer( oexINT x_nPort, PFN_OnServerEvent fnOnServerEvent = oexNULL, oexPVOID x_pData = oexNULL )
	{
		Destroy();

		m_nPort = x_nPort;
		m_pData = x_pData;
		m_fnOnServerEvent = fnOnServerEvent;

		return 0 == CThread::Start();
	}

	virtual oexBOOL InitThread( oex::oexPVOID x_pData )
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

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		// Wait for connect event
		if ( m_server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, 1000 ) )
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
				// Count a transaction
				m_nTransactions++;

				// Set the callback function for the data
				it->session.SetCallback( m_pSessionCallback, m_pSessionData );

				// Connect the port
				it->session.SetPort( &it->port );

				// Notify of new connection
				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

				// Start the thread
				it->Start();

			} // end else

		} // end if

		// Check for expired connections
		for ( typename t_LstSession::iterator it; m_lstSessions.Next( it ); )
			if ( !it->IsRunning() /* || !it->port.IsConnected() */ )
				it = m_lstSessions.Erase( it );

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

	/// Returns the number of client transactions serviced
	oexINT GetNumTransactions()
	{	return m_nTransactions; }

	/// Sets the session callback function
	void SetSessionCallback( oexPVOID x_pCallback, oexPVOID x_pData )
	{	m_pSessionCallback = x_pCallback; m_pSessionData = x_pData; }

	/// Returns the number of active sessions
	oexINT GetNumActiveClients()
	{	return m_lstSessions.Size(); }

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

};
