
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
	int GetUrl( const sqbind::stdString &sUrl, SQInteger lPort, sqbind::CSqBinary *sData );

	/// HTTP Post
	int PostUrl( const sqbind::stdString &sUrl, SQInteger lPort, const sqbind::stdString &sPost, sqbind::CSqBinary *sData );

	/// Downloads and includes the specified url
	int urlInclude( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sUrl );

	/// Downloads and inlines the specified url
	sqbind::stdString urlInline( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sUrl, sqbind::CSqMulti *pParams );

	/// Enable basic http authentication
	int SetBasicAuth( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword );

	/// Returns the last error
	sqbind::stdString GetLastError() { return m_sErr; }

	/// Returns ssl certificates if any
	sqbind::CSqMulti getCerts() { return m_mCerts; }

	/// Sets the output file
	void setFile( const sqbind::stdString &sFile ) { m_sFile = sFile; }

	/// Returns the name of the output file
	sqbind::stdString getFile() { return m_sFile; }
	
	/// Sets the connect timeout
	void setConnectTimeout( SQInteger to ) { m_nConnectTimeout = to; }
	
	/// Returns the current connect timeout
	SQInteger getConnectTimeout() { return m_nConnectTimeout; }

	/// Sets the overall transaction timeout
	void setTimeout( SQInteger to ) { m_nTimeout = to; }
	
	/// Returns the overall transaction timeout
	SQInteger getTimeout() { return m_nTimeout; }
	
private:

	/// Curl writer
	static int StdWriter( char *data, size_t size, size_t nmemb, sqbind::CSqBinary *buffer );

	static int StdFileWriter( char *data, size_t size, size_t nmemb, sqbind::CSqFile *file );


private:

	/// Curl handle
	CURL						*m_curl;

	/// String describing the last error
	sqbind::stdString			m_sErr;

	/// Username to use with connection
	sqbind::stdString			m_sUsername;

	/// Password to use with connection
	sqbind::stdString			m_sPassword;

	/// List of HTTPS certs
	sqbind::CSqMulti			m_mCerts;

	/// Sets output file
	sqbind::stdString			m_sFile;
	
	/// Sets the connect timeout CURLOPT_CONNECTTIMEOUT
	SQInteger					m_nConnectTimeout;

	/// Sets the transaction timeout CURLOPT_TIMEOUT
	SQInteger					m_nTimeout;
	
};
SQBIND_DECLARE_INSTANCE( CSqCurl, CSqCurl );

