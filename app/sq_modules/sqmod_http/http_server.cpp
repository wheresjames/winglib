// http_server.cpp

#include "stdafx.h"

void CHttpServer::SetServerCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sServer )
{	m_pServerMsgQueue = x_pMsgQueue;
	m_sServer = sServer;
}

void CHttpServer::SetSessionCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sSession )
{	m_pSessionMsgQueue = x_pMsgQueue;
	m_sSession = sSession;
}

void CHttpServer::SetSessionCallbackScript( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &sScript, int bFile, const sqbind::stdString &sSession )
{	m_pSessionMsgQueue = x_pMsgQueue;
	m_sScript = sScript;
	m_bFile = bFile;
	m_sSession = sSession;
}

int CHttpServer::Start( int nPort )
{
	// Set session callback
	m_server.SetSessionCallback( (oex::oexPVOID)CHttpServer::_OnSessionCallback, this );

	// Start the server
	if ( !m_server.StartServer( nPort, CHttpServer::_OnServerEvent, this ) )
		return 0;

	return 1;
}

oex::oexINT CHttpServer::_OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										     oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	CHttpServer *pServer = (CHttpServer*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pServer ) )
		return -2;

	return pServer->OnServerEvent( x_pData, x_nEvent, x_nErr, x_pServer );
}

oex::oexINT CHttpServer::OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										    oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	if ( m_pServerMsgQueue )
		m_pServerMsgQueue->execute( oexNULL, oexT( "." ), m_sServer );

	return 0;
}

oex::oexINT CHttpServer::_OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	CHttpServer *pServer = (CHttpServer*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pSession ) )
		return -2;

	return pServer->OnSessionCallback( x_pData, x_pSession );
}

oex::oexINT CHttpServer::OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	if ( !oexCHECK_PTR( m_pSessionMsgQueue ) || !oexCHECK_PTR( x_pSession ) )
		return -1;

	sqbind::stdString sReply;
	sqbind::stdString parGet = oex::CParser::Serialize( x_pSession->Get() ).Ptr();
	sqbind::stdString parPost = oex::CParser::Serialize( x_pSession->Post() ).Ptr();
	sqbind::stdString parHeaders = oex::CParser::Serialize( x_pSession->RxHeaders() ).Ptr();
	sqbind::stdString parRequest = oex::CParser::Serialize( x_pSession->Request() ).Ptr();

	// Are we executing a child script?
	if ( m_sScript.length() )
	{
		oex::CStr sChild = oexGuidToString();
		m_pSessionMsgQueue->spawn( &sReply, oexT( "." ), sChild.Ptr(), m_sScript, m_bFile );
		m_pSessionMsgQueue->execute( &sReply, sChild.Ptr(), m_sSession, parRequest, parHeaders, parGet, parPost );

		// Hmmm, may let the child kill it self for more flexibility
//		m_pSessionMsgQueue->kill( oexNULL, sChild.Ptr() );

	} // end if

	// Execute function in calling script
	else
		m_pSessionMsgQueue->execute( &sReply, oexT( "." ), m_sSession, parRequest, parHeaders, parGet, parPost );

//	oexSHOW( sReply.c_str() );

	sqbind::CSqMap mReply;
	mReply.deserialize( sReply );

	if ( mReply[ oexT( "content" ) ].length() )
		x_pSession->Content().Set( mReply[ oexT( "content" ) ].c_str(), mReply[ oexT( "content" ) ].length() );

	return 0;
}

