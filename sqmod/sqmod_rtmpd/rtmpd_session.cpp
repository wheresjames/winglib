
#include "stdafx.h"

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
		RTMPPacket_Free( &m_packet ), m_nPacketReady = 0;

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
	ParsePacket( &m, &m_packet.m_body[ nOffset ], m_packet.m_nBodySize - nOffset, 0 );
	return m;
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
				if ( ( p + 4 ) > e )
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
				return 0;

		} // end switch

	} // end while

	return p - s;
}

