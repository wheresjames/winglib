
// sq_curl.h

#pragma once

class CSqCurl
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqCurl )
	_SQBIND_CLASS_CTOR_END( CSqCurl )

	/// Default constructor
	CSqCurl();

	/// Destructor
	~CSqCurl();

	/// Releases resources
	void Destroy();

	/// Grab url
	int GetUrl( const sqbind::stdString &sUrl, long lPort, sqbind::CSqBinary *sData );

	/// HTTP Post
	int PostUrl( const sqbind::stdString &sUrl, long lPort, const sqbind::stdString &sPost, sqbind::CSqBinary *sData );

	/// Enable basic http authentication
	int SetBasicAuth( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword );

	/// Returns the last error
	sqbind::stdString GetLastError() { return m_sErr; }

	/// Returns ssl certificates if any
	sqbind::CSqMulti getCerts() { return m_mCerts; }

private:

	/// Curl writer
	static int StdWriter( char *data, size_t size, size_t nmemb, sqbind::CSqBinary *buffer );

private:

	/// Curl handle
	CURL						*m_curl;

	sqbind::stdString			m_sErr;

	sqbind::stdString			m_sUsername;

	sqbind::stdString			m_sPassword;

	sqbind::CSqMulti			m_mCerts;

};
SQBIND_DECLARE_INSTANCE( CSqCurl, CSqCurl );

