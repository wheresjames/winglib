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
class CHttpSession : 
    public CProtocol,
    public TBufferedPort< CAutoSocket >
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
    

public:

    /// Default constructor
    CHttpSession()
    {
        m_nErrorCode = HTTP_OK;
        m_bHeaderReceived = oexFALSE;
    }

    /// Destructor
    virtual ~CHttpSession()
    {
    }

	//==============================================================
	// OnProcessRequest()
	//==============================================================
	/// Called when an HTTP request is made
    /**
        Over-ride this and provide custom HTTP processing
    */
    virtual oexBOOL OnProcessRequest() { return oexFALSE; }

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnRead( oexINT x_nErr );

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when socket connection has been closed or aborted.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnClose( oexINT x_nErr )
    {   CloseSession();
        return oexTRUE;
    }

    /// Reads in the http headers
    oexINT ReadHeaders();

    /// Sets the default header values
    oexBOOL DefaultHeaders();

    /// Returns the content object
    CStr8& Content() { return m_sContent; }

    /// Sets the content type
    oexBOOL SetContentType( oexCSTR8 pExt );

    /// Sends the specified error message back to the client
    oexBOOL SendErrorMsg( oexINT nErrorCode, CStr8 sMsg );

    /// Returns the human readable string for the specified error code
    oexCSTR8 GetErrorString( oexINT x_nErr );

    /// Sends a reply to the client
    oexBOOL SendReply();

    /// Sets the HTTP reply code
    void SetHTTPReplyCode( oexINT nErrorCode ) { m_nErrorCode = nErrorCode; }

private:

    /// Incomming HTTP headers
    CPropertyBag8        m_pbRxHeaders;

    /// Outgoing HTTP headers
    CPropertyBag8        m_pbTxHeaders;

    /// Request information
    CPropertyBag8        m_pbRequest;

    /// Post variables
    CPropertyBag8        m_pbPost;

    /// Non-zero if the complete HTTP headers have been received.
    oexBOOL              m_bHeaderReceived;

    /// Content to return to client
    CStr8                m_sContent;

    /// Error code
    oexINT               m_nErrorCode;
};
