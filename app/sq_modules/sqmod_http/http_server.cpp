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

	// Enable sessions by default
	m_server.EnableSessions( oex::oexTRUE );

	// Multi-threaded server
	m_server.EnableMultiThreading( oex::oexFALSE );

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

	sqbind::stdString	sReply;

	// Save information
	sqbind::CSqMulti	mParams, mR;
	SQBIND_PropertyBag8ToMulti( x_pSession->Get(), mParams[ oexT( "GET" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Post(), mParams[ oexT( "POST" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->RxHeaders(), mParams[ oexT( "HEADERS" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Request(), mParams[ oexT( "REQUEST" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Session(), mParams[ oexT( "SESSION" ) ] );

	sqbind::CSqMsgQueue *q = m_pSessionMsgQueue;

	// Are we executing a child script?
	if ( m_sScript.length() )
	{
		// Create a child process to handle this transaction
//		oex::CStr sChild = oexGuidToString();

		oex::CStr sChild = x_pSession->GetTransactionId();

		q->spawn( &sReply, oexT( "." ), sChild.Ptr(), m_sScript, m_bFile );

		q = m_pSessionMsgQueue->GetQueue( sChild.Ptr() );
		if ( !q )
		{	m_pSessionMsgQueue->kill( oexNULL, sChild.Ptr() );
			oexERROR( 0, oexMks( oexT( "Failed to spawn " ), sChild ) );
			return 0;
		} // end if

	} // end if

	// Execute the thing
	q->execute( &sReply, oexT( "." ), m_sSession.c_str(), mParams.serialize() );

/*
	{ // Scope

		// Call into child
		oexAutoLock ll( q->GetLock() );
		if ( !ll.IsLocked() )
			return 0;

		// Get a pointer to the engine
		sqbind::CSqEngine *pEngine = q->GetEngine();
		if ( !pEngine )
			return 0;

		// Make the call
		if ( !pEngine->Execute( &sReply, m_sSession.c_str(), mParams.serialize(), &binReturn ) )
			return 0;

	} // end scope
*/

	// Decode the reply
	sqbind::CSqMap mReply;
	mReply.deserialize( sReply );

	// Set the content
	if ( mReply[ oexT( "content" ) ].length() )
		x_pSession->Content() =
			oexStrToMb( oex::CStr( mReply[ oexT( "content" ) ].c_str(), mReply[ oexT( "content" ) ].length() ) );

	// File as reply?
	else if ( mReply[ oexT( "file" ) ].length() )
		x_pSession->SetFileName( mReply[ oexT( "file" ) ].c_str(), mReply[ oexT( "filetype" ) ].c_str() );

	// Do we have binary data?
	else if ( mReply[ oexT( "binary" ) ].length() )
	{
		oex::CBin bin = oexGetBin( mReply[ oexT( "binary" ) ].c_str() );
		if ( bin.getUsed() )
		{	oexSetBin( mReply[ oexT( "binary" ) ].c_str(), 0 );
			x_pSession->SetBuffer( bin, mReply[ oexT( "binary_type" ) ].c_str() );
		} // end if

	} // end else if

	// Update new session data
	if ( mReply.isset( oexT( "session" ) ) )
	{
		if ( mReply[ oexT( "session" ) ].length() )
			x_pSession->Session() = oex::CParser::Deserialize( oexStrToMb( mReply[ oexT( "session" ) ].c_str() ) );
		else
			x_pSession->Session().Destroy();

	} // end if

	// Set any headers that were returned
	if ( mReply[ oexT( "headers" ) ].length() )
	{	sqbind::CSqMulti t( mReply[ oexT( "headers" ) ] );
		SQBIND_StdToPropertyBag8( t, x_pSession->TxHeaders() );
	} // end if

	if ( mReply[ oexT( "reply-code" ) ].length() )
	{	oex::oexLONG code = oexStrToLong( mReply[ oexT( "reply-code" ) ].c_str() );
		if ( code )
			x_pSession->SetHTTPReplyCode( code );
	} // end if

	return 0;
}

int CHttpServer::SetLogFile( const sqbind::stdString &sFile )
{	return m_server.SetLogFile( sFile.c_str() );
}

void CHttpServer::EnableRemoteConnections( int bEnable )
{
	// Enable/disable remote connections
	m_server.EnableRemoteConnections( bEnable ? oex::oexTRUE : oex::oexFALSE );

}

void CHttpServer::EnableSessions( int bEnable )
{
	// Enable/disable remote connections
	m_server.EnableSessions( bEnable ? oex::oexTRUE : oex::oexFALSE );

}
void CHttpServer::SetSessionTimeout( int nTimeout )
{
	if ( 0 > nTimeout )
		nTimeout = 0;

	// Enable/disable remote connections
	m_server.SetSessionTimeout( (oex::oexUINT)nTimeout );

}

void CHttpServer::EnableMultiThreading( int bEnable )
{
	m_server.EnableMultiThreading( bEnable );
}
