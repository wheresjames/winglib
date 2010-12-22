// sq_ssh2.cpp

#include "stdafx.h"

CSqSsh2::CSqSsh2()
{_STT();

	m_init = -1;
	m_session = oexNULL;

}

CSqSsh2::~CSqSsh2()
{_STT();
	Destroy();
}

void CSqSsh2::Destroy()
{_STT();

	m_sErr = oexT( "" );

	// Lose all channels
	CloseAllChannels();

	// Close session
	if ( m_session )
	{
		// Close the session
		libssh2_session_disconnect( m_session, oexT( "Normal Shutdown" ) );
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
{
	// Out with the old
	m_sErr = oexT( "" );
	Destroy();

	// Ensure valid socket
	if ( !x_pSock || !x_pSock->isConnected() || !x_pSock->Ptr() )
	{	m_sErr = oexT( "Invalid socket" );
		return 0;
	} // end if

	// Steal the socket handle
	m_socket = (int)x_pSock->Ptr()->GetSocketHandle();
	x_pSock->Ptr()->Detach();

	// Initialize SSH2
    m_init = libssh2_init( 0 );
    if ( m_init )
	{	m_sErr = sqbind::oex2std( oexMks( oexT( "Failed to initialize libssh2 : " ), m_init ) );
		Destroy();
		return 0;
	} // end if

	// Initialize a session
    m_session = libssh2_session_init();
	if ( !m_session )
	{	m_sErr = oexT( "Failed to create new ssh2 session object" );
		Destroy();
		return 0;
	} // end if

	// Execute session startup
	int nErr = 0;
    if ( ( nErr = libssh2_session_startup( m_session, m_socket ) ) )
	{	m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_session_startup() Failed : " ), nErr ) );
		Destroy();
		return 0;
	} // end if

	return 1;
}

sqbind::CSqBinary CSqSsh2::getFingerprint()
{
	sqbind::CSqBinary bin;
	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
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
{
	// Ensure session pointer
	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return oexT( "" );
	} // end if

	// Get the list
	char *pList = libssh2_userauth_list( m_session, m_sUsername.c_str(), m_sPassword.length() );
	if ( !pList )
		return oexT( "" );

	return oexMbToStrPtr( pList );
}

int CSqSsh2::authUsernamePassword( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
{
	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	int nErr = libssh2_userauth_password( m_session, oexStrToMbPtr( sUsername.c_str() ), oexStrToMbPtr( sPassword.c_str() ) );
	if ( nErr )
	{	m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_userauth_password() Failed : " ), nErr ) );
		return 0;
	} // end if

	return 1;
}

int CSqSsh2::CloseAllChannels()
{
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
{
	if ( !bin )
		return 0;

	int nSize = 4 + sHost1.length() + 4 + 4 + sHost2.length() + 4;
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
{
	if ( !sName.length() || !pMsg )
	{	m_sErr = oexT( "Invalid channel name" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	if ( 0 >= nWindowSize )
		nWindowSize = LIBSSH2_CHANNEL_WINDOW_DEFAULT;

	if ( 0 >= nPacketSize )
		nPacketSize = LIBSSH2_CHANNEL_PACKET_DEFAULT;

	// Attempt ot open the channel
	LIBSSH2_CHANNEL *ch = libssh2_channel_open_ex( m_session, 
												   sType.c_str(), sType.length(),
												   nWindowSize, nPacketSize,
												   pMsg->Ptr(), pMsg->getUsed() );
	if ( !ch )
	{	m_sErr = oexMks( oexT( "libssh2_channel_open_session() failed : " ), 
						 libssh2_session_last_errno( m_session ) ).Ptr();
		return 0;
	} // end if

	// Save in map
	m_channels[ sName ] = ch;

	return 1;
}

int CSqSsh2::OpenChannelDirectTcpip( const sqbind::stdString &sName, 
									 const sqbind::stdString &sHost, int nPort, 
									 const sqbind::stdString &sSHost, int nSPort )
{
	if ( !sName.length() )
	{	m_sErr = oexT( "Invalid channel name" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	// Attempt ot open the channel
	LIBSSH2_CHANNEL *ch = libssh2_channel_direct_tcpip_ex ( m_session, 
															sHost.c_str(), nPort,
															sSHost.c_str(), nSPort );
	if ( !ch )
	{	m_sErr = oexMks( oexT( "libssh2_channel_direct_tcpip_ex() failed : " ), 
						 libssh2_session_last_errno( m_session ) ).Ptr();
		return 0;
	} // end if

	// Save in map
	m_channels[ sName ] = ch;

	return 1;
}

int CSqSsh2::CloseChannel( const sqbind::stdString &sName )
{
	if ( !sName.length() )
	{	m_sErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sName.c_str() );
	if ( it != m_channels.end() )
	{	m_sErr = oexT( "Specified channel does not exist" );
		return 0;
	} // end if

	// Set environment variable
	int nErr = libssh2_channel_free( it->second );

	// Delete the channel from the map
	it->second = 0; m_channels.erase( it );

	if ( nErr )
	{	m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_channel_free() Failed : " ), nErr ) );
		return 0;
	} // end if

	return 1;
}

/// Sets environment variable on channel
int CSqSsh2::ChannelSetEnv( const sqbind::stdString &sChannel, const sqbind::stdString &sName, const sqbind::stdString &sValue )
{
	if ( !sChannel.length() || !sName.length() )
	{	m_sErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it != m_channels.end() )
	{	m_sErr = oexT( "Specified channel does not exist" );
		return 0;
	} // end if

	// Set environment variable
	int nErr = libssh2_channel_setenv_ex( it->second, 
										  sName.c_str(), sName.length(),
										  sValue.c_str(), sValue.length() );

	if ( nErr )
	{	m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_channel_setenv_ex() Failed : " ), nErr ) );
		return 0;
	} // end if

	return 1;
}

/// Sets environment variable on channel
int CSqSsh2::ChannelRead( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin )
{
	if ( !sChannel.length() || !bin )
	{	m_sErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it != m_channels.end() )
	{	m_sErr = oexT( "Specified channel does not exist" );
		return 0;
	} // end if

	// Set environment variable
	int nBytes = 0, nTotal = 0;
	sqbind::CSqBinary::t_byte buf[ 1024 ];
	do
	{
		// Read a block of data
		nBytes = libssh2_channel_read_ex( it->second, nStream, buf, sizeof( buf ) );
		if ( 0 < nBytes && nBytes <= sizeof( buf ) )
			bin->AppendBuffer( buf, nBytes ), nTotal += nBytes;

	} while ( 0 < nBytes && nBytes <= sizeof( buf ) );

	if ( 0 > nBytes )
		m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_channel_read_ex() Failed : " ), nBytes ) );

	return nTotal;
}

/// Sets environment variable on channel
int CSqSsh2::ChannelWrite( const sqbind::stdString &sChannel, int nStream, sqbind::CSqBinary *bin )
{
	if ( !sChannel.length() || !bin || !bin->getUsed() )
	{	m_sErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	if ( !m_session )
	{	m_sErr = oexT( "Invalid session" );
		return 0;
	} // end if

	// Ensure there is such a channel
	t_channelmap::iterator it = m_channels.find( sChannel.c_str() );
	if ( it != m_channels.end() )
	{	m_sErr = oexT( "Specified channel does not exist" );
		return 0;
	} // end if

	// Read a block of data
	int nBytes = libssh2_channel_write_ex( it->second, nStream, bin->Ptr(), bin->getUsed() );
	if ( 0 > nBytes )
		m_sErr = sqbind::oex2std( oexMks( oexT( "libssh2_channel_write_ex() Failed : " ), nBytes ) );

	return nBytes;
}
