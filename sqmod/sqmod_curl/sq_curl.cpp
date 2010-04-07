
// sq_curl.cpp

#include "stdafx.h"

CSqCurl::CSqCurl()
{_STT();
	m_curl = oexNULL;

}

CSqCurl::~CSqCurl()
{_STT();
	Destroy();
}

void CSqCurl::Destroy()
{_STT();
	m_sErr = oexT( "" );

	// Close curl handle
	if ( m_curl )
	{	curl_easy_cleanup( m_curl );
		m_curl = oexNULL;
	} // end if
}

int CSqCurl::StdWriter( char *data, size_t size, size_t nmemb, sqbind::stdString *buffer )
{_STT();
	int res = 0;
	if ( buffer )
	{	
#if defined( oexUNICODE )
		oex::CStr s = oexMbToStr( oex::CStr8( data, size * nmemb ) );
		buffer->append( s.Ptr(), s.Length() );
#else
		buffer->append( data, size * nmemb );
#endif
		res = size * nmemb;
	} // end if
	return res;
}

int CSqCurl::GetUrl( const sqbind::stdString &sUrl, long lPort, sqbind::CSqString *sData )
{_STT();
	if ( !sUrl.length() )
		return 0;

	// Lose old thing
	Destroy();

	// Initialize Curl
	m_curl = curl_easy_init();
	if ( !m_curl )
		return 0;

	curl_easy_setopt( m_curl, CURLOPT_HEADER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_FOLLOWLOCATION, 1 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0 );

	char sErr[ CURL_ERROR_SIZE ] = { 0 };
	curl_easy_setopt( m_curl, CURLOPT_ERRORBUFFER, &sErr[ 0 ] );

	// The URL we want
	curl_easy_setopt( m_curl, CURLOPT_URL, sUrl.c_str() );
	if ( lPort )
		curl_easy_setopt( m_curl, CURLOPT_PORT, lPort );

	sqbind::stdString sOut;
	curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, sData->ptr() );
	curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdWriter );

	// Do the thing
	CURLcode res = curl_easy_perform( m_curl );

	if ( CURLE_OK != res )
	{	m_sErr = oexMbToStrPtr( sErr );
		return 0;
	} // end if

	return 1;
}

int CSqCurl::PostUrl( const sqbind::stdString &sUrl, long lPort, const sqbind::stdString &sPost, sqbind::CSqString *sData )
{_STT();
	if ( !sUrl.length() )
		return 0;

	// Lose old thing
	Destroy();

	// Initialize Curl
	m_curl = curl_easy_init();
	if ( !m_curl )
		return 0;

	curl_easy_setopt( m_curl, CURLOPT_POST, 1 );

	curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, sPost.c_str() );
	curl_easy_setopt( m_curl, CURLOPT_POSTFIELDSIZE, sPost.length() );

	curl_easy_setopt( m_curl, CURLOPT_HEADER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_FOLLOWLOCATION, 1 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0 );

	char sErr[ CURL_ERROR_SIZE ] = { 0 };
	curl_easy_setopt( m_curl, CURLOPT_ERRORBUFFER, &sErr[ 0 ] );

	// The URL we want
	curl_easy_setopt( m_curl, CURLOPT_URL, sUrl.c_str() );
	if ( lPort )
		curl_easy_setopt( m_curl, CURLOPT_PORT, lPort );

	sqbind::stdString sOut;
	curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, sData->ptr() );
	curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdWriter );

	// Do the thing
	CURLcode res = curl_easy_perform( m_curl );

	if ( CURLE_OK != res )
	{	m_sErr = oexMbToStrPtr( sErr );
		return 0;
	} // end if

	return 1;
}

