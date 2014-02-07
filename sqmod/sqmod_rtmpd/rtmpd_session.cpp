
#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>

#if defined( _DEBUG )
	extern "C"
	{	extern FILE *netstackdump;
		extern FILE *netstackdump_read;
	};
#endif

// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CRtmpdSession, CRtmpdSession )

	SQBIND_MEMBER_FUNCTION( CRtmpdSession, Destroy )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, Init )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getLibVersion )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, StartDebugLog )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, StopDebugLog )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, isSession )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, ReadPacket )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getPacketType )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getPacketData )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getPacketSize )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getPacket )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, SerializePacket )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, SerializeValue )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, DeserializePacket )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, SendPacket )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, SendPacket2 )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, SendPacketBin )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, getDefaultChunkSize )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, ParseFlv )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, setNonBlockingMode )
	SQBIND_MEMBER_FUNCTION( CRtmpdSession, setTimeout )

	SQBIND_ENUM( CRtmpdSession::eFlagAllInfo, eFlagAllInfo )
	SQBIND_ENUM( CRtmpdSession::eFlagEmcaArray, eFlagEmcaArray )

SQBIND_REGISTER_CLASS_END()

void CRtmpdSession::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CRtmpdSession );
}

CRtmpdSession::CRtmpdSession()
{_STT();

	// Initialize structures
	oexZero( m_session );
	oexZero( m_packet );
	m_nPacketReady = 0;
	m_nNonBlockingMode = 0;
	m_nTs = 0;
}

void CRtmpdSession::Destroy()
{_STT();

	// Release session if any
	if ( m_session.m_sb.sb_socket )
		RTMP_Close( &m_session );

	oexZero( m_session );
	oexZero( m_packet );
	m_nPacketReady = 0;
	m_nNonBlockingMode = 0;
	m_nTs = 0;
}

SQInteger CRtmpdSession::getLibVersion()
{
	return RTMP_LibVersion();
}

int CRtmpdSession::StartDebugLog( const sqbind::stdString &sFile )
{_STT();

#if _DEBUG
	oexAutoLock ll( _g_rtmpd_lock );
	if ( !ll.IsLocked() ) return 0;

	StopDebugLog();

	oex::CStr8 s = oexStrToMb( sqbind::std2oex( sFile ) << oexT( ".dump.dat" ) );
	netstackdump = fopen( s.Ptr(), "wb" );
	if ( !netstackdump )
	{	setLastErrorStr( sqbind::stdString( oexT( "Failed to open log file : " ) ) + sqbind::oex2std( oexMbToStr( s ) ) );
		return 0;
	} // end if

	s = oexStrToMb( sqbind::std2oex( sFile ) << oexT( ".read.dat" ) );
	netstackdump_read = fopen( s.Ptr(), "wb" );
	if ( !netstackdump_read )
	{	setLastErrorStr( sqbind::stdString( oexT( "Failed to open log file : " ) ) + sqbind::oex2std( oexMbToStr( s ) ) );
		return 0;
	} // end if

	return 1;

#else
	return 0;
#endif

}

int CRtmpdSession::StopDebugLog()
{_STT();

#if _DEBUG
	oexAutoLock ll( _g_rtmpd_lock );
	if ( !ll.IsLocked() ) return 0;

	if ( netstackdump )
		fclose( netstackdump );
	if ( netstackdump_read )
		fclose( netstackdump_read );
	return 1;
#else
	return 0;
#endif

}

int CRtmpdSession::setTimeout( int nMs )
{
	// Is there a session?
	if ( !isSession() )
		return 0;

	// Ensure valid socket handle and timeout
	if ( 0 >= nMs || 0 > (long)m_session.m_sb.sb_socket )
		return 0;


#if defined( _WIN32 )

	// Set the recv and send timeouts
	return ( setsockopt( m_session.m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nMs, sizeof( nMs ) )
			 | setsockopt( m_session.m_sb.sb_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&nMs, sizeof( nMs ) )
		   ) ? 0 : 1;

#else

	// Setup socket timeout defaults
	struct timeval tv;
	tv.tv_sec = ( nMs / 1000 );
	tv.tv_usec = ( nMs % 1000 ) * 1000;

	// Set the recv and send timeouts
	return ( setsockopt( m_session.m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof( tv ) )
			 | setsockopt( m_session.m_sb.sb_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof( tv ) )
		   ) ? 0 : 1;
#endif
}

int CRtmpdSession::Init( sqbind::CSqSocket *pSocket )
{
	oexAutoLock ll( _g_rtmpd_lock );
	if ( !ll.IsLocked() ) return 0;

	// Out with the old
	Destroy();

#if _DEBUG

	// Unfortunately, this is a must for the debug version
	if ( !netstackdump || !netstackdump_read )
	{	setLastErrorStr( oexT( "You must call StartDebugLog() in debug versions" ) );
		return 0;
	} // end if

#endif

	// Sanity check
	if ( !pSocket || !pSocket->Ptr() )
	{	setLastErrorStr( oexT( "Invalid socket" ) );
		return 0;
	} // end if

	// Initialize the session object
	RTMP_Init( &m_session );

	// Mark stream as live
//	m_session.Link.lFlags |= RTMP_LF_LIVE;

	// Set short timeout
//	m_session.Link.timeout = 15;

	// Give the rtmpd object control of the socket handle
	m_session.m_sb.sb_socket = oexPtrToInt( pSocket->Ptr()->Detach() );

	// Disable Nagle's algorithm
	int on = 1;
	setsockopt( m_session.m_sb.sb_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof( on ) );

	// Attempt handshake
	if ( !RTMP_Serve( &m_session ) )
	{	setLastErrorStr( oexT( "RTMP handshake failed" ) );
		return 0;
	} // end if

	return 1;
}

int CRtmpdSession::ReadPacket()
{_STT();

	// Ensure we have a session
	if ( !isSession() )
		return -1;

	// Ensure we still have a connection
	if ( !RTMP_IsConnected( &m_session ) )
	{	Destroy();
		return -2;
	} // end if

	// Free previous packet if needed
	if ( m_nPacketReady )
		oexZero( m_packet ),
//		RTMPPacket_Free( &m_packet ), // +++ No, don't do that
		m_nPacketReady = 0;

	// Non-blocking?
	if ( m_nNonBlockingMode )
	{
		fd_set rset;
		FD_ZERO( &rset );
		FD_SET( m_session.m_sb.sb_socket, &rset );

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int ret = select( m_session.m_sb.sb_socket + 1, &rset, NULL, NULL, &tv );
		if ( 0 > ret )
			return -3;

		if ( !FD_ISSET( m_session.m_sb.sb_socket, &rset ) )
			return 0;

	} // end if

	// See if we can get a packet
	if ( !RTMP_ReadPacket( &m_session, &m_packet ) || !RTMPPacket_IsReady( &m_packet ) )
		return 0;

	// We have a packet
	m_nPacketReady = 1;

	return m_packet.m_packetType;
}

sqbind::CSqBinary CRtmpdSession::getPacketData( int nOffset )
{_STT();

	// Sanity checks
	if ( !m_nPacketReady || !m_packet.m_body
		 || 0 > nOffset || (unsigned int)nOffset >= m_packet.m_nBodySize )
		return sqbind::CSqBinary();

	// Return buffer pointer
	return sqbind::CSqBinary( &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, nOffset, 0 );
}

sqbind::CSqMulti CRtmpdSession::getPacket( int nMode )
{_STT();

	int nOffset = 0;

	// Sanity checks
	if ( !m_nPacketReady || !m_packet.m_body
		 || 0 > nOffset || (unsigned int)nOffset >= m_packet.m_nBodySize
		 || ( nMode & 0xffff0000 ) )
		return sqbind::CSqMulti();

	// Convert to array
	int i = 0;
	sqbind::CSqMulti m;

	// Process command specific data
	switch ( m_packet.m_packetType )
	{
		case 0x04 :
		{	sqbind::CSqBinary b( &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, nOffset, 0 );
			if ( 2 <= ( m_packet.m_nBodySize - nOffset ) )
				m[ sqbind::ToStr( i++ ) ].set( sqbind::ToStr( b.BE_getUSHORT( 0 ) ) ), nOffset += 2;
			while ( 4 <= ( m_packet.m_nBodySize - nOffset ) )
				m[ sqbind::ToStr( i++ ) ].set( sqbind::ToStr( b.BE_getAbsUINT( nOffset ) ) ), nOffset += 4;
			nOffset = m_packet.m_nBodySize;
		} break;

		case 0x05 :
		case 0x06 :
		{	sqbind::CSqBinary b( &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, nOffset, 0 );
			if ( 4 <= ( m_packet.m_nBodySize - nOffset ) )
				m[ sqbind::ToStr( i++ ) ].set( sqbind::ToStr( b.BE_getUINT( 0 ) ) ), nOffset += 4;
			if ( 1 <= ( m_packet.m_nBodySize - nOffset ) )
				m[ sqbind::ToStr( i++ ) ].set( sqbind::ToStr( b.BE_getUCHAR( 0 ) ) ), nOffset += 4;
		} break;

		case 0x08 :
		case 0x09 :
			nOffset = m_packet.m_nBodySize;
			break;

		// AMF3
		case 0x11 :
			if ( 1 <= ( m_packet.m_nBodySize - nOffset ) )
				nOffset++;
			break;

		default:
			break;

	} // end switch

	// Parse remaining packet data if any
	if ( m_packet.m_nBodySize > nOffset )
		ParsePacket( &m, &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, i, nMode );

	// Set packet command and type info
	switch ( m_packet.m_packetType )
	{
		case 0x01 :
			m[ oexT( "type" ) ].set( oexT( "chunkSize" ) );
			m[ oexT( "cmd" ) ].set( oexT( "chunkSize" ) );
			break;

		case 0x03 :
			m[ oexT( "type" ) ].set( oexT( "readReport" ) );
			m[ oexT( "cmd" ) ].set( oexT( "readReport" ) );
			break;

		case 0x04 :
			m[ oexT( "type" ) ].set( oexT( "control" ) );
			switch( m[ oexT( "0" ) ].toint() )
			{	case 0 : m[ oexT( "cmd" ) ].set( oexT( "beginStream" ) ); break;
				case 1 : m[ oexT( "cmd" ) ].set( oexT( "endStream" ) ); break;
				case 3 : m[ oexT( "cmd" ) ].set( oexT( "setBufferLength" ) ); break;
			} // end switch
			break;

		case 0x05 :
			m[ oexT( "type" ) ].set( oexT( "setServerBandwidth" ) );
			m[ oexT( "cmd" ) ].set( oexT( "setServerBandwidth" ) );
			break;

		case 0x06 :
			m[ oexT( "type" ) ].set( oexT( "setClientBandwidth" ) );
			m[ oexT( "cmd" ) ].set( oexT( "setClientBandwidth" ) );
			break;

		case 0x08 :
			m[ oexT( "type" ) ].set( oexT( "audio" ) );
			m[ oexT( "cmd" ) ].set( oexT( "audio" ) );
			break;

		case 0x09 :
			m[ oexT( "type" ) ].set( oexT( "video" ) );
			m[ oexT( "cmd" ) ].set( oexT( "video" ) );
			break;

		case 0x11 :
			m[ oexT( "type" ) ].set( oexT( "AMF3" ) );
			m[ oexT( "cmd" ) ].set( m[ oexT( "0" ) ].str() );
			break;

		case 0x14 :
			m[ oexT( "type" ) ].set( oexT( "AMF0" ) );
			m[ oexT( "cmd" ) ].set( m[ oexT( "0" ) ].str() );
			break;

		default :
			m[ oexT( "type" ) ].set( oexT( "unknown" ) );
			break;

	} // end swithc

	return m;
}

int CRtmpdSession::DeserializePacket( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int i, int nMode )
{_STT();
	return ParsePacket( m, bin->Mem().Ptr(), bin->Mem().getUsed(), i, nMode );
}


#define _IN_OBJECT		0x00010000
int CRtmpdSession::ParsePacket( sqbind::CSqMulti *m, const char *p, int nLength, int i, int nMode )
{_STT();

	// Sanity checks
	if ( !m || !p || 0 >= nLength )
		return 0;

	// Parse the packet data
	unsigned long l = 0;
	const char *s = p, *e = p + nLength;

	// Parse values
	while ( ( p + 1 ) < e )
	{
		// Key
		sqbind::stdString k;

		if ( !( _IN_OBJECT & nMode ) )
			k = sqbind::ToStr( i++ );
		else
		{	l = oex::os::CIpSocket::ntoh_s( *(unsigned short*)p ); p += 2;
			if ( ( p + l ) > e )
				return p - s;
			else
#if !defined( oexUNICODE )
				k.assign( p, l ), p += l;
#else
				k.assign( sqbind::oex2std( oexMbToStr( oex::CStr8( p, l ) ) ) ), p += l;
#endif
		} // end else

		switch( *p++ )
		{
			case AMF_NUMBER :

				if ( ( p + 8 ) > e )
					return p - s;
				else
				{
					oex::oexINT64 n = *(oex::oexINT64*)p; p += 8;
					oex::cmn::RevBytes( &n, sizeof( oex::oexINT64 ) );
					if ( eFlagAllInfo & nMode )
						(*m)[ k ][ oexT( "t" ) ].set( oexT( "double" ) ),
						(*m)[ k ][ oexT( "v" ) ].set( sqbind::ToStr( *(double*)&n ) );
					else
						(*m)[ k ].set( sqbind::ToStr( *(double*)&n ) );
				} // end else
				break;

			case AMF_BOOLEAN :
				if ( eFlagAllInfo & nMode )
					(*m)[ k ][ oexT( "t" ) ].set( oexT( "bool" ) ),
					(*m)[ k ][ oexT( "v" ) ].set( sqbind::ToStr( *p ? 1 : 0 ) );
				else
					(*m)[ k ].set( sqbind::ToStr( *p ? 1 : 0 ) );
				p++;
				break;

			case AMF_STRING :
				l = oex::os::CIpSocket::ntoh_s( *(unsigned short*)p ); p += 2;
				if ( ( p + l ) > e )
					return p - s;
				else
				{
					if ( eFlagAllInfo & nMode )
						(*m)[ k ][ oexT( "t" ) ].set( oexT( "string" ) ),
#if !defined( oexUNICODE )
						(*m)[ k ][ oexT( "v" ) ].set( sqbind::stdString( p, l ) );
					else
						(*m)[ k ].set( sqbind::stdString( p, l ) );
#else
						(*m)[ k ][ oexT( "v" ) ].set( sqbind::oex2std( oexMbToStr( oex::CStr8( p, l ) ) ) );
					else
						(*m)[ k ].set( sqbind::oex2std( oexMbToStr( oex::CStr8( p, l ) ) ) );
#endif
					p += l;
				} // end else
				break;

			case AMF_ECMA_ARRAY :
			case AMF_STRICT_ARRAY :

				// Skip the size
				p += 4;

			case AMF_OBJECT :
			{
				int n = 0;
				if ( eFlagAllInfo & nMode )
					(*m)[ k ][ oexT( "t" ) ].set( oexT( "obj" ) ),
					n = ParsePacket( &(*m)[ k ][ oexT( "v" ) ], p, e - p, 0, nMode | _IN_OBJECT );
				else
					n = ParsePacket( &(*m)[ k ], p, e - p, 0, nMode | _IN_OBJECT );

				if ( 0 >= n )
					return p - s;

				p += n;

			} break;

			// Ignore
			case AMF_NULL :
				break;

			// Pop
			case AMF_OBJECT_END :
				return p - s;

			// Give up on unknown item
			default :
				return p - s;

		} // end switch

	} // end while

	return p - s;
}

#define _MIN_PACKET_SIZE	1024
int CRtmpdSession::SerializeValue( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int nMode )
{
	// Sanity checks
	if ( !bin || !m )
		return 0;

	long nType = -1;
	sqbind::CSqMulti *pm = 0;

	// Did user specify types?
	if ( eFlagAllInfo & nMode )
		return 0;

	// Auto detect type
	else
	{
		// Point to the data
		pm = m;

		// Array?
		if ( m->size() )
		{
			// Get array type flags
			int nArrayType = 0;
			if ( m->isset( oexT( "_arraytype" ) ) )
			{	if ( (*m)[ oexT( "_arraytype" ) ].str() == oexT( "ecma" ) )
					nArrayType |= eFlagEmcaArray;
				if ( (*m)[ oexT( "_arraytype" ) ].str() == oexT( "strict" ) )
					nArrayType |= eFlagStrictArray;
			} // end if
			else
				nArrayType = nMode;

			// Set type flag
			if ( eFlagEmcaArray & nArrayType )
				nType = AMF_ECMA_ARRAY;
			else if ( eFlagStrictArray & nArrayType )
				nType = AMF_STRICT_ARRAY;
			else
				nType = AMF_OBJECT;

		} // end if

		else
		{
			long l = m->str().length();

			// Must have data
			if ( !l )
				nType = AMF_NULL;

			// If it's long, just assume string
			else if ( 16 < l )
				nType = AMF_STRING;

			// Boolean, must be one character, '1' or '0'
			else if ( 1 == l && ( '0' == m->str()[ 0 ] || '1' == m->str()[ 0 ] ) )
				nType = AMF_BOOLEAN;

			// Does it only contain numbers?
			else if ( sqbind::stdString::npos == m->str().find_first_not_of( oexT( "-.0123456789" ) ) )
				nType = AMF_NUMBER;

			// It's a string
			else
				nType = AMF_STRING;

		} // end else

	} // end if

	// Did we decide on anything?
	if ( 0 > nType || !pm )
		return 0;

	// Allocate default space for data
	long i = bin->getUsed();
	if ( i + _MIN_PACKET_SIZE / 2 > bin->Size() )
		if ( _MIN_PACKET_SIZE != bin->Resize( i + _MIN_PACKET_SIZE ) )
			return 0;

	// Get buffer offset
	sqbind::CSqBinary::t_byte *p = bin->Mem()._Ptr();
	if ( !p )
		return 0;

	// Save type into buffer
	p[ i++ ] = nType;

	// Encode type
	switch( nType )
	{
		default :
			return 0;

		// NULL
		case AMF_NULL :
			break;

		// NUMBER
		case AMF_NUMBER :

			// Write number and reverse the byte order
			*(double*)&p[ i ] = oexStrToDouble( pm->str().c_str() );
			oex::cmn::RevBytes( &p[ i ], 8 );

			// add sizeof( double )
			i += 8;

			break;

		// BOOLEAN
		case AMF_BOOLEAN :

			// Write boolean value
			p[ i++ ] = ( '0' != pm->str()[ 0 ] ) ? 1 : 0;

			break;

		// STRING
		case AMF_STRING :
		{
			// Get string length, and make sure it's valid
			long l = pm->str().length();
			if ( 0 >= l || 65536 <= l )
				return 0;

			// Ensure space for string
			bin->setUsed( i );
			if ( ( i + 3 + l ) > bin->Size() )
				if ( !bin->Resize( i + 3 + l + _MIN_PACKET_SIZE ) )
					return 0;
				else
					p = bin->Mem()._Ptr();

			// String length
			*(unsigned short*)&p[ i ] = oex::os::CIpSocket::hton_s( pm->str().length() ); i += 2;

			// Copy the string data
			memcpy( &p[ i ], pm->str().c_str(), l );

			// Update binary buffer length
			i += l;

		} break;

		// ARRAY
		case AMF_STRICT_ARRAY :
		case AMF_ECMA_ARRAY :

			if ( nType == AMF_STRICT_ARRAY )
				*(unsigned int*)&p[ i ] = oex::os::CIpSocket::hton_l( 1 ), i += 4,
				p[ i++ ] = AMF_OBJECT;

			else
			{
				// +++ I believe this to be correct, however, I'm trying
				// to mactch an implementation, which I believe is broken, but works.
//				int nSize = pm->size();
//				if ( 0 < nSize && pm->isset( "_arraytype" ) )
//					nSize--;

				int nSize = 0;
				*(unsigned int*)&p[ i ] = oex::os::CIpSocket::hton_l( nSize ), i += 4;

			} // end else

		// OBJECT
		case AMF_OBJECT :

			// Write data into string
			for ( sqbind::CSqMulti::iterator it = pm->begin(); it != pm->end(); it++ )
				if ( 0 < it->first.length() && '_' != it->first[ 0 ] )
				{
					// Get key length, and make sure it's valid
					long l = it->first.length();
					if ( 0 >= l || 65536 <= l )
						return 0;

					// Ensure space for Key
					bin->setUsed( i );
					if ( ( i + 3 + l ) > bin->Size() )
						if ( !bin->Allocate( i + 3 + l + _MIN_PACKET_SIZE ) )
							return 0;
						else
							p = bin->Mem()._Ptr();

					// Key length
					*(unsigned short*)&p[ i ] = oex::os::CIpSocket::hton_s( it->first.length() ); i += 2;

					// Copy the key data
					memcpy( &p[ i ], it->first.c_str(), l );

					// Update binary buffer size before setting the value
					i += l; bin->setUsed( i );

					// Write the value
					SerializeValue( bin, &it->second, nMode | eFlagStrictArray );

					// Get the new binary buffer size
					i = bin->getUsed();

				} // end for

			// End object tag
			*(unsigned short*)&p[ i ] = 0, i += 2;
			p[ i++ ] = AMF_OBJECT_END;

			break;

	} // end switch

	// Set new length
	bin->setUsed( i );

	return i;
}

int CRtmpdSession::SerializePacket( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int nMode )
{_STT();

	// Sanity checks
	if ( !m || !m->size() || !bin )
		return 0;

	long n = 0;
	while ( 0 <= n )
	{
		sqbind::stdString k = sqbind::ToStr( n++ );
		if ( !m->isset( k ) )
			n = -1;

		else
			SerializeValue( bin, &(*m)[ k ], nMode );

	} // end while

	return n;
}

int CRtmpdSession::SendPacket( sqbind::CSqMulti *m, int nQueue )
{
	// Sanity checks
	if ( !m || !m->size() || !m->isset( oexT( "pkt" ) ) )
		return 0;

	// Must have a session
	if ( !m_session.m_sb.sb_socket )
		return 0;

	// Go do the work
	return SendPacket2( (*m)[ oexT( "pkt" ) ][ oexT( "format" ) ].toint(),
						(*m)[ oexT( "pkt" ) ][ oexT( "chunk_stream_id" ) ].toint(),
						(*m)[ oexT( "pkt" ) ][ oexT( "type_id" ) ].toint(),
						(*m)[ oexT( "pkt" ) ][ oexT( "info" ) ].toint(),
						m, nQueue );
}

int CRtmpdSession::SendPacket2( int format, int csi, int type, int ext, sqbind::CSqMulti *m, int nQueue )
{
	// Sanity checks
	if ( !m || !m->size() )
		return 0;

	// Must have a session
	if ( !m_session.m_sb.sb_socket )
		return 0;

	// Initialize packet
	oexZero( m_packet );

	// Initialize packet headers
	m_packet.m_nChannel = csi;
	m_packet.m_headerType = format;
	m_packet.m_packetType = type;
	m_packet.m_nInfoField2 = ext;
/*
	// Audio
	if ( 8 == type )
		m_packet.m_nTimeStamp = m_nTs++;
	// Video
	else if ( 9 == type )
		m_packet.m_nTimeStamp = m_nTs;
	else
*/		m_packet.m_nTimeStamp = 0;

	// Timestamps?
	m_packet.m_hasAbsTimestamp = ( 0 < m_nTs ) ? 1 : 0;

	sqbind::CSqBinary body;
	if ( !body.Allocate( RTMP_MAX_HEADER_SIZE + 1024 ) )
		return 0;

	// Apparently, somewhere, RTMP_SendPacket() reaches backward
	// in the buffer, and apparently, it's by design.
	// I'm not sure how to feel about that ...

	// Add padding byte for AMF3
	if ( 0x11 == type )
		body.setUsed( RTMP_MAX_HEADER_SIZE + 1 ),
		body.setUCHAR( RTMP_MAX_HEADER_SIZE, 0 );
	else
		body.setUsed( RTMP_MAX_HEADER_SIZE );

	// Is it a raw buffer?
	if ( m->isset( oexT( "body" ) ) )
		body.appendString( (*m)[ oexT( "body" ) ].str() );

	// Serialize our data
	else if ( !SerializePacket( &body, m, 0 ) )
		return 0;

	// Point the packet at the encoded variables
	m_packet.m_body = (char*)body.Ptr( RTMP_MAX_HEADER_SIZE );

	// Set body size
	m_packet.m_nBodySize = body.getUsed() - RTMP_MAX_HEADER_SIZE;

	// Send the packet
	return RTMP_SendPacket( &m_session, &m_packet, nQueue );
}


int CRtmpdSession::SendPacketBin( int format, int csi, int type, int ext, sqbind::CSqBinary *b, int nQueue )
{
	// Sanity checks
	if ( !b )
		return 0;

	// Must have a session
	if ( !m_session.m_sb.sb_socket )
		return 0;

	// Initialize packet
	oexZero( m_packet );

	// Initialize packet headers
	m_packet.m_nChannel = csi;
	m_packet.m_headerType = format;
	m_packet.m_packetType = type;
	m_packet.m_nInfoField2 = ext;
/*
	// Audio
	if ( 8 == type )
		m_packet.m_nTimeStamp = m_nTs++;
	// Video
	else if ( 9 == type )
		m_packet.m_nTimeStamp = m_nTs;
	else
*/		m_packet.m_nTimeStamp = 0;

	// Timestamps?
	m_packet.m_hasAbsTimestamp = ( 0 < m_nTs ) ? 1 : 0;

	sqbind::CSqBinary body;
	if ( !body.Allocate( RTMP_MAX_HEADER_SIZE + b->getUsed() + 128 ) )
		return 0;

	// Add padding byte for AMF3
	if ( 0x11 == type )
		body.setUsed( RTMP_MAX_HEADER_SIZE + 1 ),
		body.setUCHAR( RTMP_MAX_HEADER_SIZE, 0 );
	else
		body.setUsed( RTMP_MAX_HEADER_SIZE );

	// Append data
	body.Append( b );

	// Point the packet at the encoded variables
	m_packet.m_body = (char*)body.Ptr( RTMP_MAX_HEADER_SIZE );

	// Set body size
	m_packet.m_nBodySize = body.getUsed() - RTMP_MAX_HEADER_SIZE;

	// Send the packet
	return RTMP_SendPacket( &m_session, &m_packet, nQueue );
}

#pragma pack( push, 1 )

	// FLV header structure
	typedef struct _SFlvHeader
	{
		char			sig[ 3 ];
		unsigned char	ver;
		unsigned char	flg;
		unsigned char	off[ 4 ];
		unsigned int	tsz;

	} SFlvHeader;

	// FLV tag structure
	typedef struct _SFlvTag
	{
		unsigned char	id;
		unsigned char	len[ 3 ];
		unsigned char	ts[ 4 ];
		unsigned char	sid[ 3 ];

	} SFlvTag;

#pragma pack( pop )

int CRtmpdSession::ParseFlv( sqbind::CSqBinary *b, sqbind::CSqMulti *m )
{_STT();

	// Param check
	if ( !b || !m )
	{	setLastErrorStr( oexT( "Bad Params" ) );
		return -1;
	} // end if

	// Get buffer pointers
	const char *p = b->Ptr();
	unsigned long l = b->getUsed(), t = b->getUsed();

	// Buffer check
	if ( !p || 0 >= l )
	{	setLastErrorStr( oexT( "Invalid buffer" ) );
		return -1;
	} // end if

	// Could it be a header?
	if ( sizeof( SFlvHeader ) <= l )
	{
		// Verify header data
		SFlvHeader *pH = (SFlvHeader*)p;
		if ( 'F' == pH->sig[ 0 ] && 'L' == pH->sig[ 1 ] && 'V' == pH->sig[ 2 ] && 1 == pH->ver
			&& !pH->off[ 0 ] && !pH->off[ 1 ] && !pH->off[ 2 ] && 9 == pH->off[ 3 ] && !pH->tsz )
			(*m)[ oexT( "header" ) ][ oexT( "flags" ) ].set( sqbind::ToStr( (int)pH->flg ) ),
			p += sizeof( SFlvHeader ), l -= sizeof( SFlvHeader );

	} // end if

	// Pointer to first tag
	long i = 0;
	SFlvTag *pTag = (SFlvTag*)p;
	while ( sizeof( SFlvTag ) <= l )
	{
		// Verify tag id
		switch ( pTag->id )
		{	case 0x08 : case 0x09 : case 0x12 :
				break;
			default :
				setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Unknown flv tag : " ), (int)pTag->id ) ) );
				return -1;
		} // end switch

		// How long is the data
		int len = ( pTag->len[ 0 ] << 16 ) | ( pTag->len[ 1 ] << 8 ) | pTag->len[ 2 ];

		// Do we have that much in the buffer?
		if ( ( sizeof( SFlvTag ) + len + 4 ) > l )
			return t - l;

		// Time stamp
		int ts = ( pTag->len[ 0 ] << 16 ) | ( pTag->len[ 1 ] << 8 ) | pTag->len[ 2 ] | ( pTag->len[ 3 ] << 24 );
		int sid = ( pTag->sid[ 0 ] << 16 ) | ( pTag->sid[ 1 ] << 8 ) | pTag->sid[ 2 ];

		// Save header information
		sqbind::CSqMulti &r = (*m)[ sqbind::ToStr( i++ ) ];
		r[ oexT( "id" ) ].set( sqbind::ToStr( pTag->id ) );
		r[ oexT( "ts" ) ].set( sqbind::ToStr( ts ) );
		r[ oexT( "sid" ) ].set( sqbind::ToStr( sid ) );
		r[ oexT( "dsz" ) ].set( sqbind::ToStr( len ) );
		r[ oexT( "dat" ) ].set( sqbind::ToStr( b->getUsed() - l + sizeof( SFlvTag ) ) );

		// Skip to next tag
		p += sizeof( SFlvTag ) + len + 4;
		l -= sizeof( SFlvTag ) + len + 4;
		pTag = (SFlvTag*)p;

	} // end while

	// Was anything decoded?
	return t - l;
}

