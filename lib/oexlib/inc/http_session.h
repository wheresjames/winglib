/*------------------------------------------------------------------
// http_session.h
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
// CHttpSession
//
/// Provides basic HTTP functionality
/**

  \code

    // Custom HTTP class
    class CCustomHttp : public CHttpSession
    {
    public:
        virtual oexBOOL OnProcessRequest()
        {   Content() << "Hello World!";
            return oexTRUE;
        }
    };

    // ...

    if ( !oexVERIFY( CIpSocket::InitSockets() ) )
        return -1;

    CAutoServer as( oexPROTOCOL( CCustomHttp ) );

    if ( !oexVERIFY( as.StartServer( 12345 ) ) )
        return -2;

    // Go ahead and navigate to http://localhost:12345/ with
    // with your web browser.

    // Waits for connection
    while ( !as.GetNumSessions() )
        os::CSys::Sleep( 15 );

    // Waits for connection to end
    while ( as.GetNumSessions() )
        os::CSys::Sleep( 15 );

    CIpSocket::UninitSockets();

  \endcode

*/
//==================================================================

template < typename T_PORT > class THttpSession
{
public:

	enum
	{ 	HTTP_OK = 200,
		HTTP_CREATED = 201,
		HTTP_ACCEPTED = 202,
		HTTP_NON_AUTHORITATIVE_INFORMATION = 203,
		HTTP_NO_CONTENT = 204,
		HTTP_RESET_CONTENT = 205,
		HTTP_PARTIAL_CONTENT = 206,

		HTTP_MULTIPLE_CHOICES = 300,
		HTTP_MOVED_PERMANENTLY = 301,
		HTTP_FOUND = 302,
		HTTP_SEE_OTHER = 303,
		HTTP_NOT_MODIFIED = 304,
		HTTP_USE_PROXY = 305,
		HTTP_TEMPORARY_REDIRECT = 307,

		HTTP_BAD_REQUEST = 400,
		HTTP_AUTHORIZATION_REQUIRED = 401,
		HTTP_PAYMENT_REQUIRED = 402,
		HTTP_FORBIDDEN = 403,
		HTTP_NOT_FOUND = 404,
		HTTP_METHOD_NOT_ALLOWED = 405,
		HTTP_NOT_ACCEPTABLE = 406,
		HTTP_PROXY_AUTHENTICATION_REQUIRED = 407,
		HTTP_REQUEST_TIMEOUT = 408,
		HTTP_CONFLICT = 409,
		HTTP_GONE = 410,
		HTTP_LENGTH_REQUIRED = 411,
		HTTP_PRECONDITION_FAILED = 412,
		HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
		HTTP_REQUEST_URI_TOO_LONG = 414,
		HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
		HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
		HTTP_EXPECTED_FAILURE = 417,

		HTTP_SERVER_ERROR = 500,
		HTTP_NOT_IMPLEMENTED = 501,
		HTTP_BAD_GATEWAY = 502,
		HTTP_SERVICE_UNAVAILABLE = 503,
		HTTP_GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505,
	};

	/// Server callback
	typedef oexINT (*PFN_Callback)( oexPVOID x_pData, THttpSession< T_PORT > *x_pSession );

public:

    /// Default constructor
    THttpSession()
    {
		m_pPort = oexNULL;
        m_nErrorCode = HTTP_OK;
        m_nTransactions = 0;
        m_bHeaderReceived = oexFALSE;
#ifdef OEX_ENABLE_ZIP
		m_bEnableCompression = oexTRUE;
#else
		m_bEnableCompression = oexFALSE;
#endif
		m_fnCallback = oexNULL;
		m_pData = oexNULL;
		m_bNewSession = oexTRUE;
		m_uSessionTimeout = 60 * 60;
    }

    THttpSession( T_PORT *x_pPort )
    {
		m_pPort = x_pPort;
        m_nErrorCode = HTTP_OK;
        m_nTransactions = 0;
        m_bHeaderReceived = oexFALSE;
#ifdef OEX_ENABLE_ZIP
		m_bEnableCompression = oexTRUE;
#else
		m_bEnableCompression = oexFALSE;
#endif
		m_fnCallback = oexNULL;
		m_pData = oexNULL;
    }

    /// Destructor
    virtual ~THttpSession()
    {	m_pPort = oexNULL;
    }

    void SetPort( T_PORT *x_pPort )
    {	m_pPort = x_pPort;
    }

    T_PORT* GetPort()
    {	return m_pPort;
    }


	//==============================================================
	// OnProcessRequest()
	//==============================================================
	/// Called when an HTTP request is made
    /**
        Over-ride this and provide custom HTTP processing
    */
    virtual oexBOOL OnProcessRequest()
    {
    	// Call the callback function if provided
		if ( m_fnCallback )
			if ( !m_fnCallback( m_pData, this ) )
				return oexTRUE;

    	return oexFALSE;
    }

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code

		\return Non-zero if success

		\see
	*/
    oexBOOL OnRead( oexINT x_nErr )
	{
		// Read in data if port is available
		if ( oexCHECK_PTR( m_pPort ) )
			Rx().Write( m_pPort->Read() );

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
		{
			// IE and Netscape append CRLF, so be sure and use
			// the content length if available
			if ( uContentLength )
				m_pbPost = CParser::DecodeUrlParams( Rx().Read( uContentLength ) );

			else
				m_pbPost = CParser::DecodeUrlParams( Rx().Read() );

		} // end if

		// Set default headers
		DefaultHeaders();

		// Do the processing
		if ( OnProcessRequest() )
			SendReply();

		// No hablo
		else
			return SendErrorMsg( HTTP_NOT_IMPLEMENTED, "Not implemented." );

		// Count a transaction
        m_nTransactions++;

		// Get ready for more requests
		m_bHeaderReceived = oexFALSE;

		return oexTRUE;
	}


	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when socket connection has been closed or aborted.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	oexBOOL OnClose( oexINT x_nErr )
    {
//    	if ( oexCHECK_PTR( m_pPort ) )
//    		m_pPort->Destroy();

        return oexTRUE;
    }

	CStr8 CreateCookie( CStr8 params )
	{	CPropertyBag8 pb;
		pb[ "OEXSID" ] = params;
		return CParser::Serialize( pb ) += "; path=/";
	}

	CStr8 DecodeCookie( CStr8 cookie )
	{	CStrList8 items = CParser::Split( m_pbRxHeaders[ "Cookie" ].ToString(), "; " );
		for ( CStrList8::iterator it; items.Next( it ); )
		{	CPropertyBag8 data = CParser::Deserialize( it.Obj() );
			if ( data.IsKey( "OEXSID" ) && data[ "OEXSID" ].ToString().Length() )
				return data[ "OEXSID" ].ToString();
		} // end for
		return CStr8();
	}

	void RestoreSession()
	{
		// Do we have session objects?
		if ( !m_ppbSession || !m_plockSession )
			return;

		// Lock the session data object
		oexAutoLock ll( m_plockSession );
		if ( !ll.IsLocked() )
			return;

		CStr8 id;

		// Is there a cookie?
		if ( m_pbRxHeaders.IsKey( "Cookie" ) )
		{
			// Get our session id
			id = DecodeCookie( m_pbRxHeaders[ "Cookie" ].ToString() );

			// Attempt to recover our data
			if ( id.Length() && m_ppbSession->IsKey( id ) )
				m_pbSession = (*m_ppbSession)[ id ].Copy();

		} // end if

		// Get remote ip address
		CStr ip = m_pPort->PeerAddress().GetDotAddress();
		oexUINT ts = (oexUINT)oexGetUnixTime();

		// Do we need to create a new session
		// Ensure session ip and port match current connection
		// This helps stop cookie spoofing
		if ( !id.Length()
			 || !m_pbSession.IsKey( "_id" ) || m_pbSession[ "_id" ].ToString() != id
			 || !m_pbSession.IsKey( "_ip" ) || m_pbSession[ "_ip" ].ToString() != ip
			 || !m_pbSession.IsKey( "_ts" )
			 || ( m_pbSession[ "_ts" ].ToULong() + m_uSessionTimeout ) < ts
			 )
		{
			// Save new connection information
			m_pbSession.Destroy();
			m_pbSession[ "_id" ] = oexGuidToString();
			m_pbSession[ "_ip" ] = ip;

		} // end if

		// Existing session restored
		else
			m_bNewSession = oexFALSE;

		// Update timestamp
		m_pbSession[ "_ts" ] = ts;

	}

	void SaveSession()
	{
		// Do we have session objects?
		if ( !m_ppbSession || !m_plockSession )
			return;

		// Lock the session data object
		oexAutoLock ll( m_plockSession );
		if ( !ll.IsLocked() )
			return;

		// Ensure session id
		if ( !m_pbSession.IsKey( "_id" ) || m_pbTxHeaders.IsKey( "Set-Cookie" ) )
			return;

		// Grab session id
		CStr id = m_pbSession[ "_id" ].ToString();

		// Erase data if session has been marked invalid
		if ( !m_pbSession[ "_ts" ].ToULong() )
			m_ppbSession->Unset( id );

		// Save the new session data
		else
			(*m_ppbSession)[ id ] = m_pbSession.Copy();

		// Add id to headers if new session
		if ( m_bNewSession )
			m_pbTxHeaders[ "Set-Cookie" ] = CreateCookie( id );
	}

	void GrabConnectionInfo()
	{
		m_pbRequest[ "REQUEST_TIME" ] = oexGetUnixTime();

		// Punt if no port
		if ( !m_pPort )
			return;

		CStr8 sLocal = m_pPort->LocalAddress().GetDotAddress();
		CStr8 sRemote = m_pPort->PeerAddress().GetDotAddress();
		m_pbRequest[ "SERVER_ADDR" ] = sLocal;
		m_pbRequest[ "SERVER_PORT" ] = m_pPort->LocalAddress().GetPort();
		m_pbRequest[ "REMOTE_ADDR" ] = sRemote;
		m_pbRequest[ "REMOTE_PORT" ] = m_pPort->PeerAddress().GetPort();
		m_pbRequest[ "IS_REMOTE" ] = ( sLocal != sRemote && sRemote != "127.0.0.1" )
										? oexT( "1" ) : oexT( "" );
	}

    /// Reads in the http headers
    oexINT ReadHeaders()
	{
		m_pbRequest.Destroy();
		m_pbRxHeaders.Destroy();
		m_bHeaderReceived = oexFALSE;

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

		// For now
		m_pbRequest[ "REQUEST_METHOD" ].ToString() = m_pbRequest[ "type" ].ToString();

		// Grab the url
		CStr8 sPath = sRx.ParseNextToken();
		m_pbRequest[ "path" ] = sPath.Parse( "?" );
		if ( m_pbRequest[ "path" ].ToString().Length() )
		{   if ( *sPath == '?' )
				sPath++;
			m_pbRequest[ "params" ] = sPath;
			m_pbGet = CParser::DecodeUrlParams( sPath );
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

		// Reconstruct the request
		m_pbRequest[ "REQUEST_STRING" ].ToString()
			<< m_pbRequest[ "type" ].ToString()
			<< " " << m_pbRequest[ "path" ].ToString();

		// Add params
		if ( m_pbRequest[ "params" ].ToString().Length() )
			m_pbRequest[ "REQUEST_STRING" ].ToString()
				<< "?" << m_pbRequest[ "params" ].ToString();

		// Protocol and version
		m_pbRequest[ "REQUEST_STRING" ].ToString()
			<< " " << m_pbRequest[ "proto" ].ToString()
			<< "/" << m_pbRequest[ "ver" ].ToString();

		// Add connection information
		GrabConnectionInfo();

		// Attempt to restore session information
		RestoreSession();

		// Headers received
		m_bHeaderReceived = oexTRUE;

		return HTTP_OK;
	}

    /// Sets the default header values
    oexBOOL DefaultHeaders()
	{
		// Lose old header values
		m_pbTxHeaders.Destroy();

		// Set the server name
		m_pbTxHeaders[ "Server" ] = "";

		// Add timestamp
		m_pbTxHeaders[ "Date" ] =
			oexStrToStr8( oexGmtTimeStr( oexT( "%w, %d %b %Y %g:%m:%s GMT" ) ) );

		// Last modified
		m_pbTxHeaders[ "Last-modified" ] = "";

		return oexTRUE;
	}

    /// Returns the content object
    CStr8& Content() { return m_sContent; }

	CStr8 GetMimeType( CStr8 x_sFile )
	{
		CStr8 sExt = x_sFile.GetFileExtension();
		if ( !sExt )
			sExt = x_sFile;

		// +++ Add MIME types
		if ( sExt == "jpg" ) return "image/jpeg";
		else if ( sExt == "png" ) return "image/png";
		else if ( sExt == "gif" ) return "image/gif";
		else if ( sExt == "htm" ) return "text/html";
		else if ( sExt == "html" ) return "text/html";
		else if ( sExt == "css" ) return "text/css";
		else if ( sExt == "txt" ) return "text/plain";
		else return "application/octet-stream";
	}

    /// Sets the content type
    oexBOOL SetContentType( CStr8 x_sFile )
	{
		// Set content type from extension / file
		m_pbTxHeaders[ "Content-type" ] = GetMimeType( x_sFile );

		return oexTRUE;
	}


    /// Sends the specified error message back to the client
    oexBOOL SendErrorMsg( oexINT nErrorCode, CStr8 sMsg )
	{
		m_nErrorCode = nErrorCode;

		Content() << "<HTML> <BODY><p><big><big>" << sMsg << "</big></big></p> </BODY></HTML>";

		// Send it
		SendReply();

		return oexTRUE;
	}


    /// Returns the human readable string for the specified error code
    oexCSTR8 GetErrorString( oexINT x_nErr )
	{
		switch( x_nErr )
		{
			case HTTP_OK :
				return "HTTP/1.0 200 OK\r\n";

			case HTTP_NO_CONTENT :
				return "HTTP/1.0 204 No Content\r\n";

			case HTTP_PARTIAL_CONTENT :
				return "HTTP/1.0 206 Partial Content\r\n";

			case HTTP_BAD_REQUEST :
				return "HTTP/1.0 400 Bad Request\r\n";

			case HTTP_AUTHORIZATION_REQUIRED :
				return "HTTP/1.0 401 Authorization Required\r\n";

			case HTTP_FORBIDDEN :
				return "HTTP/1.0 403 Forbidden\r\n";

			case HTTP_NOT_FOUND :
				return "HTTP/1.0 404 Document Not Found\r\n";

			case HTTP_REQUEST_TIMEOUT :
				return "HTTP/1.0 408 Request timed out\r\n";

			case HTTP_NOT_IMPLEMENTED :
				return "HTTP/1.0 501 Not Implemented\r\n";

			case HTTP_SERVER_ERROR :
			default :
				return "HTTP/1.0 500 Server Error\r\n";
		}

		return "HTTP/1.0 500 Server Error\r\n";
	}

	CStr GetReply()
	{
		CStr sReply;

		// How big is the data?
		m_pbTxHeaders[ "Content-length" ] = m_sContent.Length();

		// Send the header
		sReply << GetErrorString( m_nErrorCode );

		// Send the headers
		sReply << oexMbToStr( CParser::EncodeMime( m_pbTxHeaders ) ) << oexT( "\r\n" );

		// Send the content
		sReply << oexMbToStr( m_sContent );

		return sReply;
	}

    /// Sends a reply to the client
    oexBOOL SendReply()
	{
		if ( !oexCHECK_PTR( m_pPort ) )
			return oexFALSE;

		// Save session data
		SaveSession();

		// Do we need to send a file?
		if ( !m_sContent.Length() && m_sFile.Length() )
			return SendFile( m_sFile.Ptr(), m_sFileType.Ptr() );

		// For compression support
		CStr *pSend = &m_sContent;

#ifdef OEX_ENABLE_ZIP

		CStr sCompressed;

		// Is compression enabled, and does the client support it?
		if (m_bEnableCompression )
		{
			// Currently only supporting zlib/deflate
			if ( 0 <= m_pbRxHeaders[ "Accept-Encoding" ].ToString().Match( "deflate" ) )
			{	m_pbTxHeaders[ "Content-Encoding" ] = "deflate";
				sCompressed = oexCompress( m_sContent );
				pSend = &sCompressed;
			} // end if

		} // end if

#endif

		// Log the request
		Log();

		// How big is the data?
		m_pbTxHeaders[ "Content-length" ] = pSend->Length();

		// Send the header
		m_pPort->Write( GetErrorString( m_nErrorCode ) );

		// Send the headers
		m_pPort->Write( CParser::EncodeMime( m_pbTxHeaders ) << "\r\n" );

		// Send the content
		m_pPort->Write( *pSend );

		return oexTRUE;

	}

    oexBOOL SendFile( oexCSTR x_pFile, oexCSTR x_pType = oexNULL )
	{
		if ( !oexCHECK_PTR( x_pFile ) || !*x_pFile || !CFile::Exists( x_pFile ) )
			return oexFALSE;

		if ( oexCHECK_PTR( x_pType ) && *x_pType )
			SetContentType( x_pType );
		else
			SetContentType( x_pFile );

		CFile f;
		if ( !f.OpenExisting( x_pFile ).IsOpen() )
		{	oexERROR( f.GetLastError(), oexMks( oexT( "Failed to open file " ), x_pFile ) );
			return oexFALSE;
		} // end if

		// How big is the data?
		// +++ Add support for int64 to CStr
		m_pbTxHeaders[ "Content-length" ] = (oexUINT)f.Size();

		// Send the header
		m_pPort->Write( GetErrorString( m_nErrorCode ) );

		// Send the headers
		m_pPort->Write( CParser::EncodeMime( m_pbTxHeaders ) << "\r\n" );

		// Write out the file
		// Do this in chunks in case the file is huge
		oexINT64 read = 0;
		unsigned char buf[ oexSTRSIZE ];
		while ( f.Read( buf, sizeof( buf ), &read ) && read )
		{
			// Send what we can
			oexUINT uSent = 0;

			while ( uSent < read )
			{
				// Send what we can
				uSent += m_pPort->Send( &buf[ uSent ], read - uSent );

				// Wait if buffer is full
				if ( uSent < read )
				    if ( !m_pPort->WaitEvent( os::CIpSocket::eWriteEvent ) )
						return oexFALSE;

			} // end while

		} // end while

		return oexTRUE;
	}

    /// Sets the HTTP reply code
    void SetHTTPReplyCode( oexINT nErrorCode ) { m_nErrorCode = nErrorCode; }

	/// Returns a reference to the internal receive buffer
	CCircBuf& Rx()
	{	return m_rx; }

	/// Returns the number of transactions processes by this class
	oexINT GetTransactions()
	{	return m_nTransactions; }

	CPropertyBag8& Request()
	{	return m_pbRequest; }

	CPropertyBag8& RxHeaders()
	{	return m_pbRxHeaders; }

	CPropertyBag8& TxHeaders()
	{	return m_pbTxHeaders; }

	CPropertyBag8& Get()
	{	return m_pbGet; }

	CPropertyBag8& Post()
	{	return m_pbPost; }

	CPropertyBag8& Session()
	{	return m_pbSession; }

	/// Sets a callback function
	void SetCallback( PFN_Callback x_fnCallback, oexPVOID x_pData )
	{	m_fnCallback = x_fnCallback; m_pData = x_pData; }

	/// +++ Added this one to get things compiling, please change to above function
	void SetCallback( oexPVOID x_pCallback, oexPVOID x_pData )
	{	m_fnCallback = (PFN_Callback)x_pCallback; m_pData = x_pData; }

	/// Sets the log file name
	oexBOOL SetLogFile( oexCSTR x_pLog )
	{	m_sLog = x_pLog;
		return oexTRUE;
	}

	/// Sets the path to a file to send as a reply
	void SetFileName( oexCSTR pFile, oexCSTR pType = oexNULL )
	{
		if ( !oexCHECK_PTR( pFile ) )
			m_sFile.Destroy();
		else
			m_sFile = pFile;

		if ( !oexCHECK_PTR( pType ) )
			m_sFileType.Destroy();
		else
			m_sFile = pFile;
	}

	/// Returns the name of the file to be sent as a reply
	CStr GetFileName() { return m_sFile; }

	/// Reply file type
	CStr GetFileType() { return m_sFileType; }

	/// Enables / disables compression
	void EnableCompression( oexBOOL b )
	{
#ifdef OEX_ENABLE_ZIP
		m_bEnableCompression = b;
#else
		m_bEnableCompression = oexFALSE;
#endif
	}

	/// Returns non-zero if compression is enabled
	oexBOOL IsCompressionEnabled()
	{	return m_bEnableCompression; }

	CStr CommonLog()
	{
		CStr s;

		s	<< m_pbRequest[ "REMOTE_ADDR" ].ToString()
			<< " -" // rfc931
			<< " -" // username
			<< " " << oexLocalTimeStr( "[%c/%b/%Y:%g:%m:%s %Zs%Zh%Zm]" )
			<< " \"" << m_pbRequest[ "REQUEST_STRING" ].ToString() << "\""
			<< " " << m_nErrorCode
			<< " " << m_sContent.Length();

		// Add referer if specified
		if ( m_pbRxHeaders[ "referer" ].ToString().Length() )
			s << " \"" << m_pbRxHeaders[ "referer" ].ToString() << "\"";
		else
			s << " -";

		// Add user agent if specified
		if ( m_pbRxHeaders[ "User-Agent" ].ToString().Length() )
			s << " \"" << m_pbRxHeaders[ "User-Agent" ].ToString() << "\"";
		else
			s << " -";

		s << "\r\n";

		return s;
	}

	// Writes the transaction log to a file
	oexBOOL Log()
	{
		if ( !m_sLog.Length() )
			return oexFALSE;

		CFile fLog;
		if ( !fLog.OpenAlways( m_sLog.Ptr() ).IsOpen() )
			return oexFALSE;

		// Skip to the end of the file
		fLog.SetPtrPosEnd( 0 );

		// Add to the log
		return fLog.Write( CommonLog() );
	}

	/// Set sessino object
	void SetSessionObject( CPropertyBag *pPb, CLock *pLock )
	{	m_ppbSession = pPb; m_plockSession = pLock; }

	/// Returns non-zero if this is a new session
	oexBOOL IsNewSession()
	{	return m_bNewSession; }

	/// Sets the length of time that session data is to be valid
	void SetSessionTimeout( oexUINT uTo )
	{	m_uSessionTimeout = uTo; }

private:

	/// Our port
	T_PORT						*m_pPort;

    /// Incomming HTTP headers
    CPropertyBag8     			m_pbRxHeaders;

    /// Outgoing HTTP headers
    CPropertyBag8     		  	m_pbTxHeaders;

    /// Request information
    CPropertyBag8    		  	m_pbRequest;

    /// Get variables
    CPropertyBag8    		   	m_pbGet;

    /// Post variables
    CPropertyBag8     		  	m_pbPost;

    /// Post variables
    CPropertyBag8     		  	m_pbSession;

    /// Non-zero if the complete HTTP headers have been received.
    oexBOOL        		    	m_bHeaderReceived;

	/// Number of transactions processed
    oexINT						m_nTransactions;

    /// Content to return to client
    CStr8           		    m_sContent;

    /// Error code
    oexINT           		   	m_nErrorCode;

	/// Receive buffer
    CCircBuf					m_rx;

	/// Log file
	CStr						m_sLog;

	/// Name of a file to send if m_sContent is empty
	CStr						m_sFile;

	/// File type
	CStr						m_sFileType;

	/// Pointer to callback function
	PFN_Callback		    	m_fnCallback;

	/// Data passed to callback function
	oexPVOID					m_pData;

	/// Non-zero to enable compression
	oexBOOL						m_bEnableCompression;

	/// Stores session data
	CPropertyBag				*m_ppbSession;

	/// Lock for session data access
	CLock						*m_plockSession;

	/// Non-zero if a new session was just created
	oexBOOL						m_bNewSession;

	/// Length of time in seconds that session data is to be valid
	oexUINT						m_uSessionTimeout;

};
