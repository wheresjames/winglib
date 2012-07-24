
#pragma once

class CRtmpdSession
{
public:

	enum
	{
		eFlagAllInfo 		= 0x0001,

		eFlagEmcaArray		= 0x0002,

		eFlagStrictArray	= 0x0004
	};

public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CRtmpdSession )
	_SQBIND_CLASS_CTOR_END( CRtmpdSession )

	/// Default constructor
	CRtmpdSession();

	/// Destructor
	virtual ~CRtmpdSession() { Destroy(); }

	/// Copy
	CRtmpdSession( const CRtmpdSession &r ) { }
	CRtmpdSession& operator = ( const CRtmpdSession &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CRtmpdSession
		@{
	*/

	/// Destroy connection
	void Destroy();
	
	/// Initializes the RTMP session on the specified socket
	int Init( sqbind::CSqSocket *pSocket );

	/// Returns non-zero if there is a valid session
	int isSession() { return m_session.m_sb.sb_socket ? 1 : 0; }

	/// Returns the version number of the RTMPD library
	SQInteger getLibVersion();

	/// Returns the last error string
	sqbind::stdString getLastErrorStr() { return m_sLastError; }

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s ) { m_sLastError = s; }

	/// Start debug output log
	int StartDebugLog( const sqbind::stdString &sFile );

	/// Start debug output log
	int StopDebugLog();

	/// Read a packet message from the server
	/**
	
		Returns
			< 0		- Client session is closed
			= 0		- No packet or incomplete packet, call again later
			> 0		- Packet received
		
	*/
	int ReadPacket();

	/// Returns the packet type
	int getPacketType() { return m_nPacketReady ? m_packet.m_packetType : 0; }

	/// Returns the packet type
	int getPacketSize() { return m_nPacketReady ? m_packet.m_nBodySize : 0; }

	/// Returns the packet data
	sqbind::CSqBinary getPacketData( int nOffset );

	/// Returns the parsed packet data
	sqbind::CSqMulti getPacket( int nMode );

	/// Deserializes the RTMP packet into array 'm'
	int DeserializePacket( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int i, int nMode );

	/// Serializes the array 'm' into an RTMP packet
	int SerializePacket( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int nMode );

	/// Serializes the value 'm' into an RTMP packet
	int SerializeValue( sqbind::CSqBinary *bin, sqbind::CSqMulti *m, int nMode );

	/// Send a packet back to the client
	int SendPacket( sqbind::CSqMulti *m, int nQueue );

	/// Send a packet back to the client
	int SendPacket2( int format, int csi, int type, int ext, sqbind::CSqMulti *m, int nQueue );

	/// Send binary blob to the client
	int SendPacketBin( int format, int csi, int type, int ext, sqbind::CSqBinary *b, int nQueue );

	/// Returns the default chunk size
	int getDefaultChunkSize() { return RTMP_DEFAULT_CHUNKSIZE; }

	/// Parses Flv chunks
	int ParseFlv( sqbind::CSqBinary *b, sqbind::CSqMulti *m );

	/// Sets socket blocking mode
	void setNonBlockingMode( int nEnable ) { m_nNonBlockingMode = nEnable; }

	/// Sets the socket timeout in milliseconds
	int setTimeout( int nMs );

	/** @} */

	/// Parses the packet data
	int ParsePacket( sqbind::CSqMulti *m, const char *p, int nOffset, int i, int nMode );

private:

	/// RTMP session context
	RTMP 				m_session;

	/// RTMP packet
	RTMPPacket 			m_packet;

	/// Non-zero if a packet is ready
	int					m_nPacketReady;

	/// String describing the last error
	sqbind::stdString	m_sLastError;

	/// Non-zero for non-blocking mode
	int 				m_nNonBlockingMode;

};

