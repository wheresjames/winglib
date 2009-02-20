// http_server.cpp

#include "stdafx.h"

void CHttpServerImpl::SetCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sServer, const sqbind::stdString &sSession )
{	m_pMsgQueue = x_pMsgQueue;
	m_sServer = sServer;
	m_sSession = sSession;
}

int CHttpServerImpl::Start( int nPort )
{
	// Set session callback
	m_server.SetSessionCallback( (oex::oexPVOID)CHttpServerImpl::_OnSessionCallback, this );

	// Start the server
	if ( !m_server.StartServer( nPort, CHttpServerImpl::_OnServerEvent, this ) )
		return 0;

	return 1;
}

oex::oexINT CHttpServerImpl::_OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										     oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pServer ) )
		return -2;

	return pServer->OnServerEvent( x_pData, x_nEvent, x_nErr, x_pServer );
}
oex::oexINT CHttpServerImpl::OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										    oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	return 0;
}


oex::oexINT CHttpServerImpl::_OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pSession ) )
		return -2;

	return pServer->OnSessionCallback( x_pData, x_pSession );
}

oex::oexINT CHttpServerImpl::OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	if ( !oexCHECK_PTR( m_pMsgQueue ) || !oexCHECK_PTR( x_pSession ) )
		return -1;

//	sqbind::CSqMap parReply; 
	sqbind::stdString sReply;
	sqbind::stdString parGet = oex::CParser::Serialize( x_pSession->Get() ).Ptr();
	
	m_pMsgQueue->execute( &sReply, oexT( "." ), m_sSession, parGet );

	sqbind::CSqMap mReply;
	mReply.deserialize( sReply );

	if ( mReply[ oexT( "content" ) ].length() )
		x_pSession->Content() << mReply[ oexT( "content" ) ].c_str();

//	if ( oexCHECK_PTR( pReply ) )
//		x_pSession->Content() << pReply->ToString();

//	if ( parReply[ oexT( "content" ) ].length() )
//		x_pSession->Content() << parReply[ oexT( "content" ) ].c_str();

//	x_pSession->Content() << oexT( "Hello World!" );

	return 0;
}

