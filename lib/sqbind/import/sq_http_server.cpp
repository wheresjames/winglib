/*------------------------------------------------------------------
// sq_http_server.cpp
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

#include "../stdafx.h"

using namespace sqbind;

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqHttpServer, CSqHttpServer )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, Start )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, Stop )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetSessionCallback )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetSessionCallbackScript )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetAuthenticationCallback )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetServerCallback )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetLogFile )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, EnableRemoteConnections )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, EnableSessions )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, EnableMultiThreading )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, SetSessionTimeout )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, GetNumActiveClients )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, MapFolder )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, UnmapFolder )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, setPortFactory )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, getPortFactory )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, getServerId )
	SQBIND_MEMBER_FUNCTION( CSqHttpServer, setServerId )

SQBIND_REGISTER_CLASS_END()

void CSqHttpServer::Register( VM vm )
{_STT();
	_SQBIND_EXPORT( vm, CSqHttpServer );
}

void CSqHttpServer::SetServerCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sServer )
{_STT();
	m_pServerMsgQueue = x_pMsgQueue;
	m_sServer = sServer;
}

void CSqHttpServer::SetSessionCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sSession )
{_STT();
	m_pSessionMsgQueue = x_pMsgQueue;
	m_sSession = sSession;
}

void CSqHttpServer::SetAuthenticationCallback( CSqMsgQueue *x_pMsgQueue, const stdString &sAuthenticate )
{_STT();
	m_pAuthenticateMsgQueue = x_pMsgQueue;
	m_sAuthenticate = sAuthenticate;
}

void CSqHttpServer::SetSessionCallbackScript( CSqMsgQueue *x_pMsgQueue, const stdString &sScript, int bFile, const stdString &sSession )
{_STT();
	m_pSessionMsgQueue = x_pMsgQueue;
	m_sScript = sScript;
	m_bFile = bFile;
	m_sSession = sSession;
}

void CSqHttpServer::setPortFactory( CSqFactory *p )
{
	if ( p )
		m_server.SetPortFactory( p->getFactory() );
}

CSqFactory CSqHttpServer::getPortFactory()
{
	return m_server.GetPortFactory();
}

int CSqHttpServer::Start( int nPort )
{_STT();

	// Set session callback
	m_server.SetSessionCallback( (oex::oexPVOID)CSqHttpServer::_OnSessionCallback, this );

	// Set session callback
	m_server.SetAuthCallback( (oex::oexPVOID)CSqHttpServer::_OnAuthenticate, this );

	// Enable sessions by default
	m_server.EnableSessions( oex::oexTRUE );

	// Start the server
	if ( !m_server.StartServer( nPort, CSqHttpServer::_OnServerEvent, this ) )
		return 0;

	return 1;
}

void CSqHttpServer::setServerId( const stdString &sId )
{_STT();

	m_server.SetServerId( oexStrToMb( std2oex( sId ) ).Ptr() );
}

stdString CSqHttpServer::getServerId()
{_STT();

	return oex2std( oexMbToStr( m_server.GetServerId() ) );
}


int CSqHttpServer::Stop()
{_STT();

	return m_server.Stop();
}

oex::oexINT CSqHttpServer::_OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										     oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{_STT();

	CSqHttpServer *pServer = (CSqHttpServer*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pServer ) )
		return -2;

	return pServer->OnServerEvent( x_pData, x_nEvent, x_nErr, x_pServer );
}

oex::oexINT CSqHttpServer::OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										    oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{_STT();

	if ( m_pServerMsgQueue )
		m_pServerMsgQueue->execute( oexNULL, oexT( "." ), m_sServer );

	return 0;
}

oex::oexINT CSqHttpServer::_OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{_STT();

	CSqHttpServer *pServer = (CSqHttpServer*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pSession ) )
		return -2;

	return pServer->OnSessionCallback( x_pData, x_pSession );
}

oex::oexINT CSqHttpServer::OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{_STT();

	if ( !oexCHECK_PTR( m_pSessionMsgQueue ) || !oexCHECK_PTR( x_pSession ) )
		return -1;

	stdString	sReply;

	// Save information
	CSqMulti mParams, mR;
	SQBIND_PropertyBag8ToMulti( x_pSession->Get(), mParams[ oexT( "GET" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Post(), mParams[ oexT( "POST" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->RxHeaders(), mParams[ oexT( "HEADERS" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Request(), mParams[ oexT( "REQUEST" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Session(), mParams[ oexT( "SESSION" ) ] );

	CSqMsgQueue *q = m_pSessionMsgQueue;

	// Are we executing a child script?
	if ( m_sScript.length() )
	{
		// Create a child process to handle this transaction
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

	// Decode the reply
	CSqMulti mReply;
	mReply.deserialize( sReply );

	// Update new session data
	if ( mReply.isset( oexT( "session" ) ) )
	{	if ( mReply[ oexT( "session" ) ].length() )
			x_pSession->Session() 
				= oex::CParser::Deserialize( oexStrToMb( mReply[ oexT( "session" ) ].c_str() ) );
		else
			x_pSession->Session().Destroy();
	} // end if

	// Set any headers that were returned
	if ( mReply[ oexT( "headers" ) ].length() )
	{	CSqMulti t( mReply[ oexT( "headers" ) ] );
		SQBIND_StdToPropertyBag8( t, x_pSession->TxHeaders() );
	} // end if

	if ( mReply[ oexT( "reply-code" ) ].length() )
	{	oex::oexLONG code = oexStrToLong( mReply[ oexT( "reply-code" ) ].c_str() );
		if ( code )
			x_pSession->SetHTTPReplyCode( code );
	} // end if

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

	// Do we have a share buffer?
	else if ( mReply[ oexT( "share" ) ].length() )
	{
		sqbind::CSqFifoShare fs;
		sqbind::CSqBinary buf;
		oex::oexULONG uTimeout = oexGetUnixTime() + 8;

		// Multi part?
		oex::CStr8 sBoundary, sMulti;
		oex::oexCSTR pMulti = mReply[ oexT( "multi" ) ].c_str();		
		if ( !pMulti || !*pMulti )
			pMulti = 0;
		else
		{
			// Generate multipart headers
			oex::CPropertyBag8 pb;
			pb[ "Content-type" ] = pMulti;
			sMulti = oex::CParser::EncodeMIME( pb, oex::oexTRUE );

			// What will the boundary string be?
			sBoundary = std2oex( mReply[ oexT( "boundary" ) ].str() );
			if ( !sBoundary.Length() )
				sBoundary = oex::CStr( "--" ) << oexUnique();

		} // end else

		// Get MIME type
		oex::CStr sType = mReply[ oexT( "mime" ) ].c_str();
		if ( !sType.Length() ) 
		{
			// Multipart?
			if ( pMulti )
				sType = oex::CStr( "multipart/x-mixed-replace; boundary=" ) << sBoundary;
			else
				sType = oexT( "application/octet-stream" );

		} // end if

		// Set content type
		x_pSession->SetContentMimeType( oexStrToMb( sType ) );

		// Send the headers
		x_pSession->SendHeaders( -1 );

		// Add CRLF after boundary
		sBoundary += "\r\n";

		// While we're not timed out and connected
		while ( uTimeout > oexGetUnixTime() && x_pSession->IsConnected() )
		{
			// Attempt to connected to the alledged share
			if ( !fs.isOpen() )
				fs.Open( mReply[ oexT( "share" ) ].str() );

			else do
			{
				// Reset timeout
				uTimeout = oexGetUnixTime() + 60;

				// Attempt to read a data packet
				buf = fs.ReadData();

				if ( buf.getUsed() )
				{
					// Next frame
					fs.incReadPtr();

					// Handle multipart if needed
					if ( pMulti && *pMulti )
					{
						// Build multipart boundary and headers
						oex::CStr8 mpb;
						mpb << sBoundary << sMulti 
							<< "Content-Length: " << buf.getUsed()
							<< "\r\n\r\n";

						// Write multpart boundary and headers
						x_pSession->WritePort( mpb.Ptr(), mpb.Length() );

					} // end if

					// Write the new data
					x_pSession->WritePort( buf.Ptr(), buf.getUsed() );

				} // end if

			} while ( x_pSession->IsConnected() && buf.getUsed() );

			// Take a break
			oexSleep( 15 );

		} // end while

		// Signal to the writer that we're done
		if ( mReply[ oexT( "cancel_share" ) ].length() )
			fs.Cancel();

	} // end else if

	// Kill thread if we created it
	if ( !m_bScriptsLinger && m_sScript.length() )
		q->kill( oexNULL, oexT( "." ) );

	return 0;
}

int CSqHttpServer::SetLogFile( const stdString &sFile )
{_STT();
	return m_server.SetLogFile( sFile.c_str() );
}

void CSqHttpServer::EnableRemoteConnections( int bEnable )
{_STT();

	// Enable/disable remote connections
	m_server.EnableRemoteConnections( bEnable ? oex::oexTRUE : oex::oexFALSE );

}

void CSqHttpServer::EnableSessions( int bEnable )
{_STT();

	// Enable/disable remote connections
	m_server.EnableSessions( bEnable ? oex::oexTRUE : oex::oexFALSE );

}
void CSqHttpServer::SetSessionTimeout( int nTimeout )
{_STT();

	if ( 0 > nTimeout )
		nTimeout = 0;

	// Enable/disable remote connections
	m_server.SetSessionTimeout( (oex::oexUINT)nTimeout );

}

void CSqHttpServer::EnableMultiThreading( int bEnable )
{_STT();

	m_server.EnableMultiThreading( bEnable );
}

void CSqHttpServer::EnableScriptLinger( int bEnable )
{_STT();

	m_bScriptsLinger = bEnable;
}

int CSqHttpServer::GetNumActiveClients()
{_STT();

	return m_server.GetNumActiveClients();
}

int CSqHttpServer::MapFolder( const stdString &sName, const stdString &sFolder )
{_STT();

	return m_server.MapFolder( sName.c_str(), sFolder.c_str(), oex::oexTRUE );
}

int CSqHttpServer::UnmapFolder( const stdString &sName, const stdString &sFolder )
{_STT();

	return m_server.MapFolder( sName.c_str(), sFolder.c_str(), oex::oexFALSE );
}

oex::oexINT CSqHttpServer::_OnAuthenticate( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession, oex::oexLONG lType, oex::oexCSTR pData )
{_STT();

	CSqHttpServer *pServer = (CSqHttpServer*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pSession ) )
		return -2;

	return pServer->OnAuthenticate( x_pData, x_pSession, lType, pData );
}

oex::oexINT CSqHttpServer::OnAuthenticate( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession, oex::oexLONG lType, oex::oexCSTR pData )
{_STT();

	if ( !m_pAuthenticateMsgQueue )
		return 0;

	stdString sType = oexMks( lType ).Ptr();
	stdString sData = ( pData ? pData : oexT( "" ) );

	CSqMulti mParams;
	SQBIND_PropertyBag8ToMulti( x_pSession->RxHeaders(), mParams[ oexT( "HEADERS" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Request(), mParams[ oexT( "REQUEST" ) ] );
	SQBIND_PropertyBag8ToMulti( x_pSession->Session(), mParams[ oexT( "SESSION" ) ] );

	stdString sReply;
	m_pAuthenticateMsgQueue->execute( &sReply, oexT( "." ), m_sAuthenticate, sType, sData, mParams.serialize() );
	return (oex::oexINT)oexStrToLong( sReply.c_str() );
}
