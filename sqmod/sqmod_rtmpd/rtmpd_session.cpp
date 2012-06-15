
#include "stdafx.h"

#include <windows.h>
//#include <winsock2.h>

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
//	SQBIND_MEMBER_FUNCTION( CRtmpdSession,  )
//	SQBIND_MEMBER_FUNCTION( CRtmpdSession,  )
//	SQBIND_MEMBER_FUNCTION( CRtmpdSession,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CRtmpdSession );

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
}

void CRtmpdSession::Destroy()
{_STT();

	// Release session if any
	if ( m_session.m_sb.sb_socket )
		RTMP_Close( &m_session );

	oexZero( m_session );
	oexZero( m_packet );
	m_nPacketReady = 0;
}

SQInteger CRtmpdSession::getLibVersion()
{
	return RTMP_LibVersion();
}

int CRtmpdSession::StartDebugLog( const sqbind::stdString &sFile )
{_STT();

#if _DEBUG

	StopDebugLog();

	oex::CStr8 s = oexStrToMb( sqbind::std2oex( sFile ) << oexT( ".dump.dat" ) );
	netstackdump = fopen( s.Ptr(), "wb" );
	if ( !netstackdump )
	{	setLastErrorStr( sqbind::stdString( "Failed to open log file : " ) + sqbind::oex2std( s ) );
		return 0;
	} // end if

	s = oexStrToMb( sqbind::std2oex( sFile ) << oexT( ".read.dat" ) );
	netstackdump_read = fopen( s.Ptr(), "wb" );
	if ( !netstackdump_read )
	{	setLastErrorStr( sqbind::stdString( "Failed to open log file : " ) + sqbind::oex2std( s ) );
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
	if ( netstackdump )
		fclose( netstackdump );
	if ( netstackdump_read )
		fclose( netstackdump_read );
	return 1;
#else
	return 0;
#endif

}

int CRtmpdSession::Init( sqbind::CSqSocket *pSocket )
{
	// Out with the old
	Destroy();

#if _DEBUG

	// Unfortunately, this is a must for the debug version
	if ( !netstackdump || !netstackdump_read )
	{	setLastErrorStr( "You must call StartDebugLog() in debug versions" );
		return 0;
	} // end if

#endif

	// Sanity check
	if ( !pSocket || !pSocket->Ptr() )
	{	setLastErrorStr( "Invalid socket" );
		return 0;
	} // end if

	// Initialize the session object
	RTMP_Init( &m_session );

	// Give the rtmpd object control of the socket handle
	m_session.m_sb.sb_socket = oexPtrToInt( pSocket->Ptr()->Detach() );

	// Attempt handshake
	if ( !RTMP_Serve( &m_session ) )
	{	setLastErrorStr( "RTMP handshake failed" );
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
//		RTMPPacket_Free( &m_packet ), // No, don't do that
		m_nPacketReady = 0;

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
	sqbind::CSqMulti m;
	ParsePacket( &m, &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, nMode );
	return m;
}

int CRtmpdSession::DeserializePacket( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int nMode )
{_STT();
	return ParsePacket( m, bin->Mem().Ptr(), bin->Mem().getUsed(), nMode );
}


#define _USER_ALLINFO	0x00000001
#define _IN_OBJECT		0x00010000
int CRtmpdSession::ParsePacket( sqbind::CSqMulti *m, const char *p, int nLength, int nMode )
{_STT();

	// Sanity checks
	if ( !m || !p || 0 >= nLength )
		return 0;

	// Parse the packet data
	unsigned long i = 0, l = 0;
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
				k.assign( p, l ), p += l;
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
					if ( _USER_ALLINFO & nMode )
						(*m)[ k ][ "t" ].set( "double" ),
						(*m)[ k ][ "v" ].set( sqbind::ToStr( *(double*)&n ) );
					else
						(*m)[ k ].set( sqbind::ToStr( *(double*)&n ) );
				} // end else
				break;

			case AMF_BOOLEAN :
				if ( _USER_ALLINFO & nMode )
					(*m)[ k ][ "t" ].set( "bool" ),
					(*m)[ k ][ "v" ].set( sqbind::ToStr( *p ? 1 : 0 ) );
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
					if ( _USER_ALLINFO & nMode )
						(*m)[ k ][ "t" ].set( "string" ),
						(*m)[ k ][ "v" ].set( sqbind::stdString( p, l ) );
					else
						(*m)[ k ].set( sqbind::stdString( p, l ) );
					p += l;
				} // end else
				break;

			case AMF_OBJECT :
			{
				int n = 0;
				if ( _USER_ALLINFO & nMode )
					(*m)[ k ][ "t" ].set( "obj" ),
					n = ParsePacket( &(*m)[ k ][ "v" ], p, e - p, nMode | _IN_OBJECT );
				else
					n = ParsePacket( &(*m)[ k ], p, e - p, nMode | _IN_OBJECT );

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
	if ( _USER_ALLINFO & nMode )
		return 0;

	// Auto detect type
	else
	{
		// Point to the data
		pm = m;

		// Array?
		if ( m->size() )
			nType = AMF_OBJECT;

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
			else if ( sqbind::stdString::npos == m->str().find_first_not_of( "-.0123456789" ) )
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
			if ( ( i + 3 + l ) > bin->Size() )
				if ( !bin->Allocate( i + 3 + l + _MIN_PACKET_SIZE ) )
					return 0;

			// String length
			*(unsigned short*)&p[ i ] = oex::os::CIpSocket::hton_s( pm->str().length() ); i += 2;

			// Copy the string data
			memcpy( &p[ i ], pm->str().c_str(), l );

			// Update binary buffer length
			i += l;

		} break;

		// STRING
		case AMF_OBJECT :

			// Write data into string
			for ( sqbind::CSqMulti::iterator it = pm->begin(); it != pm->end(); it++ )
			{
				// Get key length, and make sure it's valid
				long l = it->first.length();
				if ( 0 >= l || 65536 <= l )
					return 0;

				// Ensure space for Key
				if ( ( i + 3 + l ) > bin->Size() )
					if ( !bin->Allocate( i + 3 + l + _MIN_PACKET_SIZE ) )
						return 0;

				// Key length
				*(unsigned short*)&p[ i ] = oex::os::CIpSocket::hton_s( it->first.length() ); i += 2;

				// Copy the key data
				memcpy( &p[ i ], it->first.c_str(), l );

				// Update binary buffer size before setting the value
				i += l; bin->setUsed( i );

				// Write the value
				SerializeValue( bin, &it->second, nMode | _IN_OBJECT );

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
	// Sanityc checks
	if ( !m || !m->size() || !m->isset( "pkt" ) )
		return 0;

	// Must have a session
	if ( !m_session.m_sb.sb_socket )
		return 0;

	// Initialize packet
	oexZero( m_packet );

	// Initialize packet headers
	m_packet.m_nChannel = (*m)[ "pkt" ][ "channel" ].toint();
	m_packet.m_headerType = (*m)[ "pkt" ][ "header_type" ].toint();
	m_packet.m_packetType = (*m)[ "pkt" ][ "packet_type" ].toint();
	m_packet.m_nTimeStamp = (*m)[ "pkt" ][ "timestamp" ].toint();
	m_packet.m_nInfoField2 = (*m)[ "pkt" ][ "infofield2" ].toint();
	m_packet.m_hasAbsTimestamp = (*m)[ "pkt" ][ "has_abs_timestamp" ].toint();

	sqbind::CSqBinary body;
	if ( !body.Allocate( RTMP_MAX_HEADER_SIZE + 1024 ) )
		return 0;
	
	// Apparently, somewhere, RTMP_SendPacket() reaches backward 
	// in the buffer, and apparently, it's by design.
	// I'm not sure how to feel about that ...
	body.setUsed( RTMP_MAX_HEADER_SIZE );

	// Serialize our data
	if ( !SerializePacket( &body, m, 0 ) )
		return 0;

	// Point the packet at the encoded variables
	m_packet.m_body = (char*)body.Ptr( RTMP_MAX_HEADER_SIZE );

	// Set body size
	m_packet.m_nBodySize = body.getUsed() - RTMP_MAX_HEADER_SIZE;

	// Send the packet
	return RTMP_SendPacket( &m_session, &m_packet, nQueue );
}

