// sq_ssh2.h

#pragma once

class CSqSsh2
{
public:

	/// Channel map type
	typedef oexStdMap( sqbind::stdString, LIBSSH2_CHANNEL* ) t_channelmap;

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqSsh2 )
	_SQBIND_CLASS_CTOR_END( CSqSsh2 )

	/// Default constructor
	CSqSsh2();

	/// CSqSsh2
	~CSqSsh2();

	/// Releases resources
	void Destroy();

	/// Initializes the ssh connection to the server
	int Connect( sqbind::CSqSocket *x_pSock );

	/// Enable / disable blocking mode
	int setBlockingMode( int bEnable );

	/// Returns the error description for the specified code
	sqbind::stdString getErrorDesc( int nCode );

	/// Returns a string describing the last error
	sqbind::stdString getLastErrorStr();

	/// Returns a string describing the last error
	sqbind::stdString getLastError() { return m_sErr; }

	/// Returns the last error code
	int getLastErrorCode() { return m_nErr; }

	/// Returns the connected hosts fingerprint
	sqbind::CSqBinary getFingerprint();

	/// Returns a list of the authorization methods available
	sqbind::stdString getAuthorizationMethods();

	/// Set username / password information
	void setUserInfo( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
	{	m_sUsername = sUsername; m_sPassword = sPassword; }

	/// Attempts authentication using username and password
	int authUsernamePassword( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword );

	/// Attempts authentication using public key
	int authPublicKey( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword,
					   const sqbind::stdString &sPrvKey, const sqbind::stdString &sPubKey );

	/// Attempts authentication using public key
	int authPublicKeyBin( const sqbind::stdString &sUsername,
						  sqbind::CSqBinary *pPrv, sqbind::CSqBinary *pPub );

	/// Sign callback proxy
	static int _sign_callback( LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
							   const unsigned char *data, size_t data_len, void **user );

	/// Sign data callback
	static int sign_callback( LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
							  const unsigned char *data, size_t data_len );

public:

	/// Close all channels
	int CloseAllChannels();

	/// Opens the specified channel
//	int OpenChannel( const sqbind::stdString &sName );
	int OpenChannel( const sqbind::stdString &sName, const sqbind::stdString &sType,
					 int nWindowSize, int nPacketSize, sqbind::CSqBinary *pMsg );

	/// Closes the specified channel
	int CloseChannel( const sqbind::stdString &sName );

	/// Sets environment variable on channel
	int ChannelSetEnv( const sqbind::stdString &sChannel, const sqbind::stdString &sName, const sqbind::stdString &sValue );

	/// Reads data from the specified channel
	int ChannelRead( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin, int timeout );

	/// Writes data to the specified channel
	int ChannelWrite( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin, int timeout );

	/// Creates port forward message
	int MsgFwd( sqbind::CSqBinary *bin, const sqbind::stdString &sHost1, int nHost1, const sqbind::stdString &sHost2, int nHost2 );

	/// Opens a direct tcpip port forward
	int OpenChannelDirectTcpip( const sqbind::stdString &sName, 
								const sqbind::stdString &sHost, int nPort, 
								const sqbind::stdString &sSHost, int nSPort );

	/// Log generic error
	int logerr( int nRet, int nErr, const sqbind::stdString &sF );

	/// Echo's SSH debug information to terminal
	int print_debug_information( int bEnable );

private:

	/// Init status
	int							m_init;

	/// The socket handle to use
	int							m_socket;

	/// SSH Session
    LIBSSH2_SESSION				*m_session;

	/// Channel map
	t_channelmap				m_channels;

	/// Username
	sqbind::stdString			m_sUsername;

	/// Password
	sqbind::stdString			m_sPassword;

	/// Message describing the last error
	sqbind::stdString			m_sErr;

	/// Last error code
	int							m_nErr;

};
SQBIND_DECLARE_INSTANCE( CSqSsh2, CSqSsh2 );

