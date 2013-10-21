// sq_ssh2.cpp

#include "stdafx.h"

#if !defined( OEX_WINDOWS )
#	include <unistd.h>
#	include <sys/socket.h>
#endif

#define SQSSH2_RETRY( e, f )																	\
	{	unsigned int nTo = oexGetUnixTime() + ( oexDEFAULT_WAIT_TIMEOUT / 1000 );				\
		do { e = f; if ( e == LIBSSH2_ERROR_EAGAIN && nTo > oexGetUnixTime() ) oexSleep( 15 );	\
		} while ( e == LIBSSH2_ERROR_EAGAIN && nTo > oexGetUnixTime() );						\
	}

// +++ This doesn't work right, need to figure out non-blocking in libssh2
#define SQSSH2_RETRY2( e, r, s, f )																\
	{	unsigned int nTo = oexGetUnixTime() + ( oexDEFAULT_WAIT_TIMEOUT / 1000 );				\
		r = f;																					\
		do { e = libssh2_session_last_errno( s );												\
		 if ( e == LIBSSH2_ERROR_EAGAIN && nTo > oexGetUnixTime() )								\
			e = _libssh2_wait_socket( s );														\
		} while ( e == LIBSSH2_ERROR_EAGAIN && nTo > oexGetUnixTime() );						\
	}

// From libssh2/src/session.h
extern "C" int _libssh2_wait_socket( LIBSSH2_SESSION *session );

#if defined( LIBSSH2DEBUG_USERCALLBACK ) && !defined( LIBSSH2DEBUG )
static int CSqSsh2_echo_debug_msgs = 0;
// From libssh2/src/misc.c
extern "C" void _libssh2_debug(LIBSSH2_SESSION * session, int context, const char *format, ...)
{
	if ( CSqSsh2_echo_debug_msgs )
	{
		oex::CStr sRes;
		
		if ( format )
		{	oexVaList ap; oexVaStart( ap, format );
			sRes.vPrint( ( oex::CStr() << oexT( "CSqSsh2 : " ) << oexMbToStrPtr( format ) ).Ptr(), ap );
			oexVaEnd( ap );
		} // end if

		oexEcho( sRes.Ptr() );

	} // end if


}
#endif

int CSqSsh2::print_debug_information( int bEnable )
{
#if defined( LIBSSH2DEBUG_USERCALLBACK ) && !defined( LIBSSH2DEBUG )
	CSqSsh2_echo_debug_msgs = bEnable;
	return 1;
#else
	return 0;
#endif
}

CSqSsh2::CSqSsh2()
{_STT();

	m_init = -1;
	m_nErr = 0;
	m_session = oexNULL;

}

CSqSsh2::~CSqSsh2()
{_STT();
	Destroy();
}

int CSqSsh2::logerr( int nRet, int nErr, const sqbind::stdString &sF )
{_STT();

	m_nErr = nErr;
	m_sErr = sF;

	if ( nErr )
		m_sErr += oexT( " : " ),
		m_sErr += sqbind::oex2std( oexMks( nErr ) ),
		m_sErr += oexT( " : " ),
		m_sErr += getErrorDesc( nErr );

	if ( m_session )
		m_sErr += oexT( " : " ),
		m_sErr += sqbind::oex2std( oexMks( libssh2_session_last_errno( m_session ) ) ),
		m_sErr += oexT( " : " ),
		m_sErr += getLastErrorStr();

	return nRet;
}


void CSqSsh2::Destroy()
{_STT();

	m_nErr = 0;
	m_sErr = oexT( "" );

	// Lose all channels
	CloseAllChannels();

	// Close session
	if ( m_session )
	{
		// Close the session
		libssh2_session_disconnect( m_session, "Normal Shutdown" );
		libssh2_session_free( m_session );
		m_session = oexNULL;

	} // end if

	if ( m_socket )
	{
#if defined( OEX_WINDOWS )
	    closesocket( (SOCKET)m_socket );
#else
		close( m_socket );
#endif
	} // end if

	// Close ssh
	libssh2_exit();
	m_init = -1;

}

int CSqSsh2::Connect( sqbind::CSqSocket *x_pSock )
{_STT();
	// Out with the old
	Destroy();

	// Ensure valid socket
	if ( !x_pSock || !x_pSock->isConnected() || !x_pSock->Ptr() )
		return logerr( 0, 0, oexT( "Invalid socket" ) );

	// Steal the socket handle
	m_socket = oexPtrToInt( x_pSock->Ptr()->GetSocketHandle() );
	x_pSock->Ptr()->Detach();

	// Initialize SSH2
    m_init = libssh2_init( 0 );
    if ( m_init )
	{	Destroy();
		logerr( 0, m_init, oexT( "Failed to initialize libssh2 : libssh2_init()" ) );		
		return 0;
	} // end if

	// Initialize a session
    m_session = libssh2_session_init();
	if ( !m_session )
	{	Destroy();
		logerr( 0, 0, oexT( "Failed to create new ssh2 session object : libssh2_session_init()" ) );
		return 0;
	} // end if

	// Execute session startup
	int nErr = 0;
    if ( 0 > ( nErr = libssh2_session_startup( m_session, m_socket ) ) )
	{	Destroy();
		logerr( 0, nErr, oexT( "libssh2_session_startup() failed" ) );
		return 0;
	} // end if

	// Turn off blocking
//	libssh2_session_set_blocking( m_session, 0 );

	return 1;
}

sqbind::CSqBinary CSqSsh2::getFingerprint()
{_STT();
	sqbind::CSqBinary bin;
	if ( !m_session )
	{	logerr( 0, 0, oexT( "Invalid session" ) );
		return bin;
	} // end if

	// Allocate space for hash
	if ( !bin.Allocate( 20 ) )
		return bin;

	// Copy the hash
	bin.MemCpy( libssh2_hostkey_hash( m_session, LIBSSH2_HOSTKEY_HASH_SHA1 ), 20 );

	return bin;
}


sqbind::stdString CSqSsh2::getAuthorizationMethods()
{_STT();
	// Ensure session pointer
	if ( !m_session )
	{	logerr( 0, 0, oexT( "Invalid session" ) );
		return oexT( "" );
	} // end if

	// Get the list
	int nErr = 0;
	char *pList = 0; // libssh2_userauth_list( m_session, m_sUsername.c_str(), m_sUsername.length() );
#if !defined( oexUNICODE )
	SQSSH2_RETRY2( nErr, pList, m_session, libssh2_userauth_list( m_session, oexStrToMbPtr( m_sUsername.c_str() ), m_sUsername.length() ) );
#else
	oex::CStr8 sUsername8 = sqbind::std2oex8( m_sUsername );
	SQSSH2_RETRY2( nErr, pList, m_session, libssh2_userauth_list( m_session, sUsername8.Ptr(), sUsername8.Length() ) );
#endif

	if ( 0 > nErr ) 
	{	logerr( 0, nErr, oexT( "libssh2_userauth_list() Failed" ) ); return oexT( "" ); }

	if ( !pList )
		return oexT( "" );

	return oexMbToStrPtr( pList );
}

int CSqSsh2::authUsernamePassword( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
{_STT();
	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	int nErr = 0;
	SQSSH2_RETRY( nErr, libssh2_userauth_password( m_session, oexStrToMbPtr( sUsername.c_str() ), oexStrToMbPtr( sPassword.c_str() ) ) );

	if ( 0 > nErr ) 
		return logerr( 0, nErr, oexT( "libssh2_userauth_password() Failed" ) );

	return 1;
}

int CSqSsh2::authPublicKey( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword,
							const sqbind::stdString &sPrvKey, const sqbind::stdString &sPubKey )
{_STT();
	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	int nErr = 0;
	SQSSH2_RETRY( nErr, libssh2_userauth_publickey_fromfile_ex( m_session, 
																oexStrToMbPtr( sUsername.c_str() ),
																sUsername.length(),
																sPubKey.length() ? oexStrToMbPtr( sPubKey.c_str() ) : oexNULL,
																oexStrToMbPtr( sPrvKey.c_str() ),
																sPassword.length() ? oexStrToMbPtr( sPassword.c_str() ) : 0 ) );

	if ( 0 > nErr ) 
		return logerr( 0, nErr, oexT( "libssh2_userauth_publickey_fromfile_ex() Failed" ) );

	return 1;
}

int CSqSsh2::authPublicKeyBin( const sqbind::stdString &sUsername,
							   sqbind::CSqBinary *pPrv, sqbind::CSqBinary *pPub )
{_STT();

	if ( !sUsername.length() || !pPrv || !pPub || !pPrv->getUsed() || !pPub->getUsed() )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	int nErr = 0;
	SQSSH2_RETRY( nErr, libssh2_userauth_publickey( m_session,
													oexStrToMbPtr( sUsername.c_str() ),
													(const unsigned char*)pPub->Ptr(), pPub->getUsed(),
													_sign_callback, (void**)this ) );

	if ( 0 > nErr ) 
		return logerr( 0, nErr, oexT( "libssh2_userauth_publickey() Failed" ) );

	return 1;
}

int CSqSsh2::_sign_callback( LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
							 const unsigned char *data, size_t data_len, void **user )
{_STT();
	CSqSsh2 *p = (CSqSsh2*)user;
	if ( !p ) return LIBSSH2_ERROR_BAD_USE;
	return sign_callback( session, sig, sig_len, data, data_len );
}

int CSqSsh2::sign_callback( LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
							const unsigned char *data, size_t data_len )
{_STT();



	return 0;
}

int CSqSsh2::CloseAllChannels()
{_STT();
	// For each channel
	if ( m_session )
		for ( t_channelmap::iterator it = m_channels.begin(); it != m_channels.end(); it++ )
			if ( it->second )
				libssh2_channel_free( it->second ), it->second = 0;

	// Lose all channel mappings
	m_channels.clear();

	return 1;
}

int CSqSsh2::MsgFwd( sqbind::CSqBinary *bin, 
					 const sqbind::stdString &sHost1, int nHost1, 
					 const sqbind::stdString &sHost2, int nHost2 )
{_STT();
	if ( !bin )
		return 0;

	unsigned int nSize = 4 + sHost1.length() + 4 + 4 + sHost2.length() + 4;
	if ( bin->Size() < nSize )
		if ( !bin->Allocate( nSize ) )
			return 0;

	sqbind::CSqBinary::t_byte *p = bin->_Ptr();
	if ( !p )
		return 0;

	oex::cmn::setBE( p, (oex::oexUINT32)sHost1.length() ), p += 4;
	oexMemCpy( p, sHost1.c_str(), sHost1.length() ), p += sHost1.length();
	oex::cmn::setBE( p, (oex::oexUINT32)nHost1 ), p += 4;

	oex::cmn::setBE( p, (oex::oexUINT32)sHost2.length() ), p += 4;
	oexMemCpy( p, sHost2.c_str(), sHost2.length() ), p += sHost2.length();
	oex::cmn::setBE( p, (oex::oexUINT32)nHost2 ), p += 4;

	bin->setUsed( nSize );

	return nSize;
}


int CSqSsh2::OpenChannel( const sqbind::stdString &sName, const sqbind::stdString &sType,
						  int nWindowSize, int nPacketSize, sqbind::CSqBinary *pMsg )
{_STT();
	if ( !sName.length() || !pMsg )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	if ( 0 >= nWindowSize )
		nWindowSize = LIBSSH2_CHANNEL_WINDOW_DEFAULT;

	if ( 0 >= nPacketSize )
		nPacketSize = LIBSSH2_CHANNEL_PACKET_DEFAULT;

	// Attempt ot open the channel
	int nErr = 0;
	LIBSSH2_CHANNEL *ch = 0;
#if !defined( oexUNICODE )
	SQSSH2_RETRY2( nErr, ch, m_session, libssh2_channel_open_ex( m_session, 
																 sType.c_str(), sType.length(),
																 nWindowSize, nPacketSize,
																 pMsg->Ptr(), pMsg->getUsed() ) );
#else
	oex::CStr8 sType8 = sqbind::std2oex8( sType );
	SQSSH2_RETRY2( nErr, ch, m_session, libssh2_channel_open_ex( m_session, 
																 sType8.Ptr(), sType8.Length(),
																 nWindowSize, nPacketSize,
																 pMsg->Ptr(), pMsg->getUsed() ) );
#endif
	if ( !ch && 0 > nErr )
		return logerr( 0, nErr, oexT( "libssh2_channel_direct_tcpip_ex() failed" ) );

	if ( !ch )
		return logerr( 0, 0, oexT( "libssh2_channel_open_session() returned null pointer" ) );

	// Save in map
	m_channels[ sName ] = ch;

	return 1;
}

int CSqSsh2::setBlockingMode( int bEnable )
{
	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	// Disable blocking
	libssh2_session_set_blocking( m_session, bEnable );

	return 1;
}

int CSqSsh2::OpenChannelDirectTcpip( const sqbind::stdString &sName, 
									 const sqbind::stdString &sHost, int nPort, 
									 const sqbind::stdString &sSHost, int nSPort )
{_STT();

	if ( !sName.length() )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

//	libssh2_session_set_blocking( m_session, 0 );

	int nErr = 0;
	LIBSSH2_CHANNEL *ch = 0;
	
#if !defined( oexUNICODE )
	SQSSH2_RETRY2( nErr, ch, m_session, libssh2_channel_direct_tcpip_ex( m_session, 
																		 sHost.c_str(), nPort,
																		 sSHost.c_str(), nSPort ) );
#else
	oex::CStr8 sHost8 = sqbind::std2oex8( sHost ), sSHost8 = sqbind::std2oex8( sSHost );
	SQSSH2_RETRY2( nErr, ch, m_session, libssh2_channel_direct_tcpip_ex( m_session, 
																		 sHost8.Ptr(), nPort,
																		 sSHost8.Ptr(), nSPort ) );
#endif																		 
	if ( 0 > nErr )
		return logerr( 0, nErr, oexT( "libssh2_channel_direct_tcpip_ex() failed" ) );

	if ( !ch )
		return logerr( 0, 0, oexT( "libssh2_channel_direct_tcpip_ex() returned null pointer" ) );

	// Turn off blocking on this channel, ( this function currently just calls libssh2_session_set_blocking() )
	libssh2_channel_set_blocking( ch, 0 );

	// Save in map
	m_channels[ sName ] = ch;

	return 1;
}

int CSqSsh2::CloseChannel( const sqbind::stdString &sName )
{_STT();
	if ( !sName.length() )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sName.c_str() );
	if ( it != m_channels.end() )
		return logerr( 0, 0, oexT( "Specified channel does not exist" ) );

	// Set environment variable
	int nErr = libssh2_channel_free( it->second );

	// Delete the channel from the map
	it->second = 0; m_channels.erase( it );

	if ( nErr )
		return logerr( 0, nErr, oexT( "libssh2_channel_free() Failed" ) );

	return 1;
}

/// Sets environment variable on channel
int CSqSsh2::ChannelSetEnv( const sqbind::stdString &sChannel, const sqbind::stdString &sName, const sqbind::stdString &sValue )
{_STT();
	if ( !sChannel.length() || !sName.length() )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it == m_channels.end() )
		return logerr( 0, 0, oexT( "Specified channel does not exist" ) );

	// Set environment variable
#if !defined( oexUNICODE )
	int nErr = libssh2_channel_setenv_ex( it->second, 
										  sName.c_str(), sName.length(),
										  sValue.c_str(), sValue.length() );
#else
	oex::CStr8 sName8 = sqbind::std2oex8( sName ), sValue8 = sqbind::std2oex8( sValue );
	int nErr = libssh2_channel_setenv_ex( it->second, 
										  sName8.Ptr(), sName8.Length(),
										  sValue8.Ptr(), sValue8.Length() );
#endif

	if ( nErr )
		return logerr( 0, nErr, oexT( "libssh2_channel_setenv_ex() Failed" ) );

	return 1;
}

/// Sets environment variable on channel
int CSqSsh2::ChannelRead( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin, int timeout )
{_STT();
	if ( !sChannel.length() || !bin )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it == m_channels.end() )
		return logerr( 0, 0, oexT( "Specified channel does not exist" ) );

	// Set environment variable
	int nBytes = 0, nTotal = 0;
	if ( 0 > timeout ) timeout = oexDEFAULT_WAIT_TIMEOUT / 1000;
	unsigned int uTo = ( 0 < timeout ) ? oexGetUnixTime() + timeout : 0;
	sqbind::CSqBinary::t_byte buf[ 1024 ];
	do
	{
		// Read a block of data
//		SQSSH2_RETRY( libssh2_channel_read_ex( it->second, nStream, buf, sizeof( buf ) ) );
//		nBytes = nErr;
		nBytes = libssh2_channel_read_ex( it->second, nStream, buf, sizeof( buf ) );
		if ( 0 < nBytes && (unsigned int)nBytes <= sizeof( buf ) )
			bin->AppendBuffer( buf, nBytes ), nTotal += nBytes;

		// Wait a bit
		if ( LIBSSH2_ERROR_EAGAIN == nBytes )
			oexSleep( 15 );

	} while ( ( nBytes == sizeof( buf ) || LIBSSH2_ERROR_EAGAIN == nBytes )
		      && uTo && uTo > oexGetUnixTime() );

	if ( 0 > nBytes )
		logerr( 0, nBytes, oexT( "libssh2_channel_read_ex() Failed" ) );

	return nTotal;
}

/// Sets environment variable on channel
#define CSQSSH2_MAX_WRITE_BLOCK		( 32 * 1024 )
int CSqSsh2::ChannelWrite( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin, int timeout )
{_STT();

	if ( !sChannel.length() || !bin || !bin->getUsed() )
		return logerr( 0, 0, oexT( "Invalid parameters" ) );

	if ( !m_session )
		return logerr( 0, 0, oexT( "Invalid session" ) );

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it == m_channels.end() )
		return logerr( 0, 0, oexT( "Specified channel does not exist" ) );

	// Read a block of data
//	SQSSH2_RETRY( libssh2_channel_write_ex( it->second, nStream, bin->Ptr(), bin->getUsed() ) );
	int nSent = 0;
	if ( 0 > timeout ) timeout = oexDEFAULT_WAIT_TIMEOUT / 1000;
	unsigned int uTo = ( 0 < timeout ) ? oexGetUnixTime() + timeout : 0;
	do
	{
		// Seems to have problems if we try really large writes
		int nMax = bin->getUsed() - nSent;
		if ( nMax > CSQSSH2_MAX_WRITE_BLOCK )
			nMax = CSQSSH2_MAX_WRITE_BLOCK;

		// Attempt to send some data
		int nBytes = libssh2_channel_write_ex( it->second, nStream, bin->Ptr( nSent ), nMax );
		
		// Just ignore try again messages if timeout
		if ( LIBSSH2_ERROR_EAGAIN == nBytes && uTo )
			nBytes = 0;

		// Was there an error?
		if ( 0 > nBytes )
		{	logerr( 0, nBytes, oexT( "libssh2_channel_write_ex() Failed" ) );
			return ( 0 < nSent ) ? nSent : nBytes;
		} // end if

		// Don't hog the cpu if the write thing is busy
		if ( nBytes < nMax )
			oexSleep( 15 );

		// Add number of bytes sent
		nSent += nBytes;

	} while( nSent < bin->getUsed() && uTo && uTo > oexGetUnixTime() );

	return nSent;
}

sqbind::stdString CSqSsh2::getLastErrorStr()
{_STT();
	if ( !m_session )
		return oexT( "No active session" );

	char *str = 0; int len = 0;
	libssh2_session_last_error( m_session, &str, &len, 0 );
	if ( !str || 0 >= len )
		return oexT( "Unspecified error" );

	return sqbind::oex2std( oexMbToStr( oex::CStr8( str, len ) ) );
}

sqbind::stdString CSqSsh2::getErrorDesc( int nCode )
{_STT();
	
	switch ( nCode )
	{
		case LIBSSH2_ERROR_NONE :					return oexT( "None" );
		case LIBSSH2_ERROR_SOCKET_NONE :			return oexT( "Socket Error" );
		case LIBSSH2_ERROR_BANNER_RECV :			return oexT( "Banner failed to receive" );
		case LIBSSH2_ERROR_BANNER_SEND :			return oexT( "Banner failed to send" );
		case LIBSSH2_ERROR_INVALID_MAC :			return oexT( "Invalid MAC" );
		case LIBSSH2_ERROR_KEX_FAILURE :			return oexT( "Key failure" );
		case LIBSSH2_ERROR_ALLOC :					return oexT( "Memory allocation failure" );
		case LIBSSH2_ERROR_SOCKET_SEND :			return oexT( "Socket send() failed" );
		case LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE :	return oexT( "Key exchange failed" );
		case LIBSSH2_ERROR_TIMEOUT :				return oexT( "Timeout" );
		case LIBSSH2_ERROR_HOSTKEY_INIT :			return oexT( "Host key init failed" );
		case LIBSSH2_ERROR_HOSTKEY_SIGN :			return oexT( "Host key sign failed" );
		case LIBSSH2_ERROR_DECRYPT :				return oexT( "Decrypt failed" );
		case LIBSSH2_ERROR_SOCKET_DISCONNECT :		return oexT( "Socket disconnect" );
		case LIBSSH2_ERROR_PROTO :					return oexT( "Proto failed" );
		case LIBSSH2_ERROR_PASSWORD_EXPIRED :		return oexT( "Password has expired" );
		case LIBSSH2_ERROR_FILE :					return oexT( "File error" );
		case LIBSSH2_ERROR_METHOD_NONE :			return oexT( "Method failed" );
		case LIBSSH2_ERROR_AUTHENTICATION_FAILED :	return oexT( "Authentication failed" );
//		case LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED : return oexT( "Public key not recognized" );
		case LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED :	return oexT( "Unable to verify public key" );
		case LIBSSH2_ERROR_CHANNEL_OUTOFORDER :		return oexT( "Channel out of order" );
		case LIBSSH2_ERROR_CHANNEL_FAILURE :		return oexT( "Channel failure" );
		case LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED : return oexT( "Channel request denied" );
		case LIBSSH2_ERROR_CHANNEL_UNKNOWN :		return oexT( "Unspecified channel error" );
		case LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED : return oexT( "Channel window exceeded" );
		case LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED : return oexT( "Channel packet exceeded" );
		case LIBSSH2_ERROR_CHANNEL_CLOSED :			return oexT( "Channel closed" );
		case LIBSSH2_ERROR_CHANNEL_EOF_SENT :		return oexT( "Channel EOF sent" );
		case LIBSSH2_ERROR_SCP_PROTOCOL :			return oexT( "SCP protocol error" );
		case LIBSSH2_ERROR_ZLIB :					return oexT( "ZLIB error" );
		case LIBSSH2_ERROR_SOCKET_TIMEOUT :			return oexT( "Socket timeout" );
		case LIBSSH2_ERROR_SFTP_PROTOCOL :			return oexT( "SFTP Protocol error" );
		case LIBSSH2_ERROR_REQUEST_DENIED :			return oexT( "Request denied" );
		case LIBSSH2_ERROR_METHOD_NOT_SUPPORTED :	return oexT( "Method not supported" );
		case LIBSSH2_ERROR_INVAL :					return oexT( "Inval error" );
		case LIBSSH2_ERROR_INVALID_POLL_TYPE :		return oexT( "Invalid poll type" );
		case LIBSSH2_ERROR_PUBLICKEY_PROTOCOL :		return oexT( "Public key protocol error" );
		case LIBSSH2_ERROR_EAGAIN :					return oexT( "Error, try again" );
		case LIBSSH2_ERROR_BUFFER_TOO_SMALL :		return oexT( "Buffer is too small" );
		case LIBSSH2_ERROR_BAD_USE :				return oexT( "Invalid parameter" );
		case LIBSSH2_ERROR_COMPRESS :				return oexT( "Compress error" );
		case LIBSSH2_ERROR_OUT_OF_BOUNDARY :		return oexT( "Out of boundary" );
		case LIBSSH2_ERROR_AGENT_PROTOCOL :			return oexT( "Agent protocol error" );
		case LIBSSH2_INIT_NO_CRYPTO :				return oexT( "Init, No crypto" );
		case LIBSSH2_ERROR_SOCKET_RECV :			return oexT( "Socket recieve error" );
		case LIBSSH2_ERROR_ENCRYPT :				return oexT( "Encryption error" );
		case LIBSSH2_ERROR_BAD_SOCKET :				return oexT( "Bad Socket" );
	};

	return oexT( "Unknown" );
}
