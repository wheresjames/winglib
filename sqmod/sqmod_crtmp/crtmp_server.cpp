
#include "stdafx.h"

// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CRtmpServer, CRtmpServer )

	SQBIND_MEMBER_FUNCTION( CRtmpServer, Destroy )
	SQBIND_MEMBER_FUNCTION( CRtmpServer, Init )
	SQBIND_MEMBER_FUNCTION( CRtmpServer, Start )
	SQBIND_MEMBER_FUNCTION( CRtmpServer, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CRtmpServer, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CRtmpServer, getProtocolMap )
//	SQBIND_MEMBER_FUNCTION( CRtmpServer,  )
//	SQBIND_MEMBER_FUNCTION( CRtmpServer,  )
//	SQBIND_MEMBER_FUNCTION( CRtmpServer,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CRtmpServer );

void CRtmpServer::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CRtmpServer );
}

CRtmpServer::CRtmpServer()
{_STT();

	m_pSa = 0;
	m_pPf = 0;
	m_pPh = 0;
	m_pTcpAcceptor = 0;
	m_nIoInit = 0;
	m_nProtoInit = 0;

}

void CRtmpServer::Destroy()
{_STT();

	if ( 0 < m_nProtoInit )
	{
		// Shutdown protocol manager
		m_nProtoInit = 0;
		ProtocolManager::Shutdown();
		ProtocolManager::CleanupDeadProtocols();
	} // end if

	if ( 0 < m_nIoInit )
	{
		// Stop io manager
		m_nIoInit = 0;
		IOHandlerManager::ShutdownIOHandlers();
		IOHandlerManager::DeleteDeadHandlers();
		IOHandlerManager::Shutdown();
	} // end if

	// Delete the protocol factory
	if ( m_pPf )
	{	ProtocolFactoryManager::UnRegisterProtocolFactory( m_pPf );
		delete m_pPf;
		m_pPf = 0;
	} // end if

	if ( m_pPh )
	{	delete m_pPh;
		m_pPh = 0;
	} // end if

	if ( m_pTcpAcceptor )
	{	delete m_pTcpAcceptor;
		m_pPh = 0;
	} // end if

	if ( m_pSa )
	{	BaseClientApplication::Shutdown( m_pSa ); // Calls delete :(
		m_pSa = 0;
	} // end if

}

int CRtmpServer::Init()
{
//	oexAutoLock ll( _g_crtmp_lock );
//	if ( !ll.IsLocked() ) return 0;

oexEcho( "0" );

	// Out with the old
	Destroy();

	// Initialize I/O
	m_nIoInit = 0;
	IOHandlerManager::Initialize();

oexEcho( "a" );

	// Configuration
	m_cConfig[ CONF_APPLICATION_NAME ] = "RTMP Server Module";
	m_cConfig[ CONF_APPLICATION_MEDIAFOLDER ] = "/dummy";

	// Create RTMP app object
	m_pSa = new CRtmpServerApp( m_cConfig );
	if ( !m_pSa )
	{	Destroy();
		setLastErrorStr( "Unable to allcoate memory for app object" );
		return 0;
	} // end if

oexEcho( "b" );

	// Create protol handler
	m_pPh = new CRTMPAppProtocolHandler( m_cConfig );
	if ( !m_pPh )
	{	Destroy();
		setLastErrorStr( "Unable to allcoate memory for protocol handler" );
		return 0;
	} // end if

	// Register RTMP protocol
	m_pSa->RegisterAppProtocolHandler(PT_INBOUND_RTMP, m_pPh);
	m_pSa->RegisterAppProtocolHandler(PT_INBOUND_RTMPS_DISC, m_pPh);
	m_pSa->RegisterAppProtocolHandler(PT_OUTBOUND_RTMP, m_pPh);

oexEcho( "c" );

	// Create default protocol handler
	m_pPf = new DefaultProtocolFactory();
	if ( !m_pPf )
	{	Destroy();
		setLastErrorStr( "Unable to allcoate memory for protocol factory" );
		return 0;
	} // end if

	// Protocol factory
	if ( !ProtocolFactoryManager::RegisterProtocolFactory( m_pPf ) )
	{	setLastErrorStr( "Failed to register default protocol factory" );
		return 0;
	} // end if

	return 1;
}

int CRtmpServer::Start( int x_nPort )
{
	if ( !m_pSa || !m_pPh )
	{	setLastErrorStr( "Cannot start server without a successfull call to Init()" );
		return 0;
	} // end if

	// Start i/o manager
	IOHandlerManager::Start();

	vector<uint64_t> chain;
	chain = ProtocolFactoryManager::ResolveProtocolChain("rtmp");
	if (0 >= chain.size())
	{	Destroy();
		setLastErrorStr( "Unable to resolve protocol chain for : RTMP" );
		return 0;
	} // end if
	
	// Create TCP acceptor
	m_pTcpAcceptor = new TCPAcceptor( 0, x_nPort, m_cConfig, chain);
	if ( !m_pTcpAcceptor )
	{	Destroy();
		setLastErrorStr( "Unable to allcoate memory for server socket" );
		return 0;
	} // end if

	// Bind server socket
	if ( !m_pTcpAcceptor->Bind() )
	{	Destroy();
		setLastErrorStr( sqbind::oex2std( oex::CStr("Failed to bind port : " ) += x_nPort ) );
		return 0;
	} // end if

oexEcho( "d" );

	return 1;
}

sqbind::stdString CRtmpServer::getProtocolMap()
{
	return sqbind::StrCvt(ProtocolFactoryManager::Dump());
}


