// http_server.h

#pragma once

class CHttpServer
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CHttpServer )
	_SQBIND_CLASS_CTOR_END( CHttpServer )

	/// Default constructor
	CHttpServer()
	{
		m_pServerMsgQueue = oexNULL;
		m_pSessionMsgQueue = oexNULL;
		m_bScriptsLinger = 0;
		m_bFile = 0;
	}

	// Copy semantics
	CHttpServer( const CHttpServer &r ) {}
	CHttpServer& operator =( const CHttpServer &r ) { return *this; }

	void SetServerCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sServer );

	void SetSessionCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sSession );

	void SetSessionCallbackScript( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sScript, int bFile, const sqbind::stdString &sSession );

	virtual int Start( int nPort );

	/// Server callback
	static oex::oexINT _OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
								       oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer );
	oex::oexINT OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
						       oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer );

	/// Session callback
	static oex::oexINT _OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );
	oex::oexINT OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession );

	/// Starts web logging
	int SetLogFile( const sqbind::stdString &sFile );

	/// Enable / Disable remote connections
	void EnableRemoteConnections( int bEnable );

	/// Enable / Disable session data
	void EnableSessions( int bEnable );

	/// Length of time in seconds that session data is valid
	void SetSessionTimeout( int nTimeout );

	/// Enable / Disable multi-threaded sessions
	void EnableMultiThreading( int bEnable );

	/// Non-zero to enable scripts to linger
	void EnableScriptLinger( int bEnable );

private:

	// The server
	oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > m_server;

	// Server callback queue
	sqbind::CSqMsgQueue		*m_pServerMsgQueue;

	// Session callback queue
	sqbind::CSqMsgQueue		*m_pSessionMsgQueue;

	// Server callback function
	sqbind::stdString		m_sServer;

	// Session callback function
	sqbind::stdString		m_sSession;

	// Callback script
	sqbind::stdString		m_sScript;

	/// Non-zero if m_sScript holds a file name
	oex::oexBOOL			m_bFile;

	/// Non-zero if scripts should not be killed after 
	oex::oexBOOL			m_bScriptsLinger;
};
