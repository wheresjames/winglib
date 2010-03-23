/*------------------------------------------------------------------
// http_session.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

oexBOOL CHttpSession::OnRead( oexINT x_nErr )
{
    // Process the incomming data
    if ( !TBufferedPort< CAutoSocket >::OnRead( x_nErr ) )
	    return oexFALSE;

    // Grab headers if needed
    if ( !m_bHeaderReceived )
    {
        oexINT nErr = ReadHeaders();

        // Waiting for data?
        if ( !nErr ) 
            return oexFALSE;

        // Error?
        if ( HTTP_OK != nErr )
            return SendErrorMsg( nErr, "Bad HTTP request headers." );

    } // end if

    // Do we have all the data?
    oexUINT uContentLength = m_pbRxHeaders[ "Content-Length" ].ToLong();
    if ( uContentLength && uContentLength > Rx().GetMaxRead() )
        return oexFALSE;

    // Are there any post variables?
    if ( !m_pbPost.Size() && m_pbRequest[ "type" ] == "POST" )
        m_pbPost = CParser::DecodeUrlParams( Rx().Read() );

    // Set default headers
    DefaultHeaders();

    // Do the processing
    if ( OnProcessRequest() )
        SendReply();

    // No hablo
    else 
        return SendErrorMsg( HTTP_NOT_IMPLEMENTED, "Not implemented." );

    // Get ready for more requests
    m_bHeaderReceived = oexFALSE;

    return oexTRUE;
}

oexINT CHttpSession::ReadHeaders()
{
    m_pbRequest.Destroy();
    m_pbRxHeaders.Destroy();

    // See if we have any data
    CStr8 sRx = Rx().Peek();
    if ( !sRx.Length() )
        return 0;

    // See if we have a full set of headers
    oexINT nEndHeaders = sRx.Match( "\r\n\r\n" );
    if ( 0 > nEndHeaders ) 
        return 0;

    // Drop everything after the headers in our local buffer
    // and remove the headers from the incomming buffer
    sRx.SetLength( nEndHeaders );    
	Rx().AdvanceReadPtr( nEndHeaders + 4 );

    // Get header type
    m_pbRequest[ "type" ] = CParser::ParseToken( sRx, CStrList8() << "GET" << "POST" << "HEAD", oexFALSE );
    if ( !m_pbRequest[ "type" ].ToString().Length() )
        return HTTP_BAD_REQUEST;

    // Grab the url
    CStr8 sPath = sRx.ParseNextToken();
    m_pbRequest[ "path" ] = sPath.Parse( "?" ); 
    if ( m_pbRequest[ "path" ].ToString().Length() )
    {   if ( *sPath == '?' ) 
            sPath++;
        m_pbRequest[ "params" ] = CParser::DecodeUrlParams( sPath );
    } // end if
    else m_pbRequest[ "path" ].ToString() = sPath;

    // Grab the protocol / must be HTTP
    m_pbRequest[ "proto" ] = CParser::ParseToken( sRx.SkipWhiteSpace(), CStrList8() << "HTTP", oexFALSE );
    if ( !m_pbRequest[ "proto" ].ToString().Length() || *sRx != oexT( '/' ) )
        return HTTP_BAD_REQUEST;

    // Skip /
    sRx++;

    // Grab the version
    m_pbRequest[ "ver" ] = sRx.ParseNextToken();

    // Go to the next line
    sRx.NextLine();

    // Read in the headers
    m_pbRxHeaders = CParser::DecodeMIME( sRx );

    // Headers received
    m_bHeaderReceived = oexTRUE;

    return HTTP_OK;
}

oexBOOL CHttpSession::DefaultHeaders()
{
	// Lose old header values
	m_pbTxHeaders.Destroy();

	// Set the server name
	m_pbTxHeaders[ "Server" ] = "";

    // Add timestamp
    m_pbTxHeaders[ "Date" ] = 
        oexStrToStr8( CSysTime().GetSystemTime().FormatTime( oexT( "%w, %d %b %Y %g:%m:%s GMT" ) ) );

	// Last modified
	m_pbTxHeaders[ "Last-modified" ] = "";

    return oexTRUE;
}

oexBOOL CHttpSession::SetContentType( oexCSTR8 pExt )
{
    // +++ Add MIME types
//	if ( !CMime::GetContentType( pExt, type ) )
//		strcpy( type, "text/html" );

    m_pbTxHeaders[ "Content-type" ] = "text/html";

	return oexTRUE;
}


oexBOOL CHttpSession::SendErrorMsg( oexINT nErrorCode, CStr8 sMsg )
{	
    m_nErrorCode = nErrorCode;

    Content() << "<HTML> <BODY><p><big><big>" << sMsg << "</big></big></p> </BODY></HTML>";

	// Send it
	SendReply();

	return oexTRUE;
}

oexCSTR8 CHttpSession::GetErrorString( oexINT x_nErr )
{
	switch( x_nErr )
	{
		case HTTP_OK :
			return "HTTP/1.0 200 OK\r\n";
			break;

		case HTTP_NO_CONTENT :
			return "HTTP/1.0 204 No Content\r\n";
			break;

		case HTTP_PARTIAL_CONTENT :
			return "HTTP/1.0 206 Partial Content\r\n";
			break;

        case HTTP_BAD_REQUEST :
            return "HTTP/1.0 400 Bad Request\r\n";
            break;

		case HTTP_FORBIDDEN :
			return "HTTP/1.0 403 Forbidden\r\n";
			break;
		
		case HTTP_NOT_FOUND :
			return "HTTP/1.0 404 Document Not Found\r\n";
			break;

		case HTTP_REQUEST_TIMEOUT :
			return "HTTP/1.0 408 Request timed out\r\n";
			break;

        case HTTP_NOT_IMPLEMENTED :
			return "HTTP/1.0 501 Not Implemented\r\n";
			break;

		case HTTP_SERVER_ERROR :
		default :
			return "HTTP/1.0 500 Server Error\r\n";
			break;
	}

	return "HTTP/1.0 500 Server Error\r\n";
}

    // Apperently compression doesn't work
//  m_pbTxHeaders[ "Transfer-encoding" ] = "gzip";
//  Write( zip::CCompress::Compress( m_sContent ) );

oexBOOL CHttpSession::SendReply()
{
	// How big is the data?
	m_pbTxHeaders[ "Content-length" ] = m_sContent.Length();

	// Send the header
    Write( GetErrorString( m_nErrorCode ) );

    // Send the headers
    Write( CParser::EncodeMime( m_pbTxHeaders ) << "\r\n" );

    // Send the content
    Write( m_sContent );

	return oexTRUE;

}


