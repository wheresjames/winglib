/*------------------------------------------------------------------
// sq_http_server.h
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

// namespace
namespace sqbind
{
	class CSqHttpServer
	{
	public:

		_SQBIND_CLASS_CTOR_BEGIN( CSqHttpServer )
		_SQBIND_CLASS_CTOR_END( CSqHttpServer )

		/// Default constructor
		CSqHttpServer()
		{
			m_pServerMsgQueue = oexNULL;
			m_pSessionMsgQueue = oexNULL;
			m_pAuthenticateMsgQueue = oexNULL;
			m_bScriptsLinger = 0;
			m_bFile = 0;
		}

		// Copy semantics
		CSqHttpServer( const CSqHttpServer &r ) {}
		CSqHttpServer& operator =( const CSqHttpServer &r ) { return *this; }

		virtual ~CSqHttpServer() { Stop(); }

		static void Register( VM vm );

		void SetServerCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sServer );

		void SetSessionCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sSession );

		void SetAuthenticationCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sAuthenticate );

		void SetSessionCallbackScript( CSqMsgQueue *x_pMsgQueue, const stdString &sScript, int bFile, const stdString &sSession );

		virtual int Start( int nPort );

		virtual int Stop();

		/// Server callback
		static oex::oexINT _OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										   oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer );
		oex::oexINT OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
								   oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer );

		/// Session callback
		static oex::oexINT _OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );
		oex::oexINT OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );

		/// Authentication callback
		static oex::oexINT _OnAuthenticate( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession, oex::oexLONG lType, oex::oexCSTR pData );
		oex::oexINT OnAuthenticate( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession, oex::oexLONG lType, oex::oexCSTR pData );

		/// On Close session callback
		static oex::oexINT _OnCloseSession( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );
		oex::oexINT OnCloseSession( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );

		/// Starts web logging
		int SetLogFile( const stdString &sFile );

		/// Enable / Disable remote connections
		void EnableRemoteConnections( int bEnable );

		/// Enable / Disable session data
		void EnableSessions( int bEnable );

		/// Length of time in seconds that session data is valid
		void SetSessionTimeout( int nTimeout );

		/// Enable / Disable multi-threaded sessions
		void EnableMultiThreading( int bEnable );

		/// Enables / disables compression
		int EnableCompression( int bEnable );

		/// Non-zero to enable scripts to linger
		void EnableScriptLinger( int bEnable );

		/// Returns the number of active client connections
		int GetNumActiveClients();

		/// Map folder
		int MapFolder( const stdString &sName, const stdString &sFolder );

		/// Unmap folder
		int UnmapFolder( const stdString &sName, const stdString &sFolder );

		/// Sets the server id
		void setServerId( const stdString &sId );

		/// Sets the specified default header
		void setDefaultHeader( const stdString &sName, const stdString &sValue )
		{	m_server.DefHeaders()[ oexStrToMb( sqbind::std2oex( sName ) ) ] = oexStrToMb( sqbind::std2oex( sValue ) ); }

		/// Returns the value of the specified default header
		sqbind::stdString getDefaultHeader( const stdString &sName )
		{	return sqbind::oex2std( m_server.DefHeaders()[ oexStrToMb( sqbind::std2oex( sName ) ) ].ToString() ); }

		/// Returns all the default headers
		sqbind::CSqMulti getDefaultHeaders();
		
		/// Returns the server id
		stdString getServerId();

		/// Sets the port factory on the server
		void setPortFactory( CSqFactory *p );

		/// Sets the port factory on the server
		CSqFactory getPortFactory();

		/// Sets the maximum number of connections
		void setMaxConn( int n ) { m_server.setMaxConn( n ); }

		/// Returns the maximum number of connections
		int getMaxConn() { return m_server.getMaxConn(); }

		/// Sets the maximum number of connections that can be queued
		void setMaxQueue( int n ) { m_server.setMaxQueue( n ); }

		/// Returns the maximum number of connections that can be queued
		int getMaxQueue() { return m_server.getMaxQueue(); }

	private:

		// The server
		oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > m_server;

		// Server callback queue
		CSqMsgQueue		*m_pServerMsgQueue;

		// Session callback queue
		CSqMsgQueue		*m_pSessionMsgQueue;

		// Session callback queue
		CSqMsgQueue		*m_pAuthenticateMsgQueue;

		// Server callback function
		stdString		m_sServer;

		// Session callback function
		stdString		m_sSession;

		// Authentication callback function
		stdString		m_sAuthenticate;

		// Callback script
		stdString		m_sScript;

		/// Non-zero if m_sScript holds a file name
		oex::oexBOOL	m_bFile;

		/// Non-zero if scripts should not be killed after
		oex::oexBOOL	m_bScriptsLinger;
	};

};

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqHttpServer, CSqHttpServer )
