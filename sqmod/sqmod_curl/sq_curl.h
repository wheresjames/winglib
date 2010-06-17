
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
	int GetUrl( const sqbind::stdString &sUrl, long lPort, sqbind::CSqString *sData );

	/// HTTP Post
	int PostUrl( const sqbind::stdString &sUrl, long lPort, const sqbind::stdString &sPost, sqbind::CSqString *sData );

	/// Enable basic http authentication
	int SetBasicAuth( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword );

	/// Returns the last error
	sqbind::stdString GetLastError() { return m_sErr; }

private:

	/// Curl writer
	static int StdWriter( char *data, size_t size, size_t nmemb, sqbind::stdString *buffer );

private:

	/// Curl handle
	CURL						*m_curl;

	sqbind::stdString			m_sErr;

	sqbind::stdString			m_sUsername;

	sqbind::stdString			m_sPassword;

};
SQBIND_DECLARE_INSTANCE( CSqCurl, CSqCurl );

