
#include "stdafx.h"

#include "asiolist.h"

// +++ ????
// oexSTATIC_ASSERT( sizeof( ASIOSampleRate ) == sizeof( double ) );

// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CAsioDrv, CAsioDrv )

	SQBIND_MEMBER_FUNCTION( CAsioDrv, Destroy )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getDriverError )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getDevices )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, isDriver )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, LoadDriver )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, OpenChannels )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, CloseChannels )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getInfo )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getSampleRate )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setSampleRate )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, Start )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, Stop )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getDriverSignal )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getInputs )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getOutputs )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getSamples )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getSampleType )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setCbId )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getCbId )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setPrefix )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getPrefix )
//	SQBIND_MEMBER_FUNCTION( CAsioDrv,  )

SQBIND_REGISTER_CLASS_END()

void CAsioDrv::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CAsioDrv );
}

// +++ Eliminate this one day
oexLock		CAsioDrv::m_lock;
CAsioDrv	*CAsioDrv::m_pInst = 0;

CAsioDrv::CAsioDrv()
{_STT();

	m_nId = -1;
	m_pDriver = 0;
	m_nSignal = 0;
	m_nSamples = 0;
	m_nInputs = 0;
	m_nOutputs = 0;
	m_uCbId = 0;
	m_nSampleType = 0;
	m_nOpenChannels = 0;
	*m_adi.errorMessage = 0;
}

void CAsioDrv::Destroy()
{_STT();

	// Close any open channels
	CloseChannels();

	// Close driver if open
	if ( isDriver() )
		m_adl.asioCloseDriver( m_nId );

	// Reset
	m_nId = -1;
	m_pDriver = 0;
	m_nSignal = 0;
	m_nInputs = 0;
	m_nOutputs = 0;

	*m_adi.name = 0;
	*m_adi.errorMessage = 0;
	m_adi.driverVersion = 0;
}

#define MAX_ASIO_DRIVERS 256
#define MAX_ASIO_DRIVER_NAME_LEN 256
sqbind::CSqMulti CAsioDrv::getDevices()
{_STT();

	AsioDriverList adl;
	char name[ MAX_ASIO_DRIVER_NAME_LEN ];

	// How many drivers does the system think it has?
	long nDrivers = adl.asioGetNumDev();
	if ( 0 >= nDrivers )
		return sqbind::CSqMulti();
	else if ( MAX_ASIO_DRIVERS < nDrivers )
		nDrivers = MAX_ASIO_DRIVERS;

	// Get each driver's info
	sqbind::CSqMulti m;
	for( long i = 0; i < nDrivers; i++ )
		if ( !adl.asioGetDriverName( i, name, MAX_ASIO_DRIVER_NAME_LEN - 1 ) )
			name[ MAX_ASIO_DRIVER_NAME_LEN - 1 ] = 0,
			m[ sqbind::oex2std( oexMks( i ) ) ][ oexT( "name" ) ].set( oexMbToStrPtr( name ) );
		else
			i = nDrivers;

	return m;
}

sqbind::stdString CAsioDrv::getDriverError()
{	if ( !m_pDriver )
		return oexT( "Driver not loaded" );
	char szMsg[ 256 ] = { 0 };
	m_pDriver->getErrorMessage( szMsg );
#if defined( oexUNICODE )
	return sqbind::oex2std( oexMbToStr( szMsg ) );
#else
	return szMsg;
#endif
}


int CAsioDrv::LoadDriver( int nId )
{_STT();

	// Out with the old
	Destroy();

	// Ensure valid driver id
	if ( 0 > nId )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Invalid driver id : " ), nId ) ) );
		return 0;
	} // end if

	// Open the driver
	long err = m_adl.asioOpenDriver( nId, (void**)&m_pDriver );
	if ( ASE_OK != err || !m_pDriver )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "asioOpenDriver() failed : " ),
												  nId, oexT( " : " ), err,
												  oexT( " : " ), getDriverError().c_str() ) ) );
		return 0;
	} // end if

	if ( !m_pDriver->init( m_adi.sysRef ) )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "ASIOInit() failed : " ),
												  nId, oexT( " : " ), err,
												  oexT( " : " ), getDriverError().c_str() ) ) );
		Destroy();
		return 0;
	} // end if

	// Save away the number of inputs / outputs
	m_nInputs = 0; m_nOutputs = 0;
	err = m_pDriver->getChannels( &m_nInputs, &m_nOutputs );
	if ( ASE_OK != err )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getChannels() failed : " ),
												  nId, oexT( " : " ), err,
												  oexT( " : " ), getDriverError().c_str() ) ) );
		Destroy();
		return 0;
	} // end if

	m_cb.bufferSwitch = &CAsioDrv::_onBufferSwitch;
	m_cb.sampleRateDidChange = &CAsioDrv::_onSampleRateDidChange;
	m_cb.asioMessage = &CAsioDrv::_onAsioMessage;
	m_cb.bufferSwitchTimeInfo = &CAsioDrv::_onBufferSwitchTimeInfo;

	// Save driver id
	m_nId = nId;

	return 1;
}

void CAsioDrv::setSampleRate( double d )
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return;
	} // end if

	ASIOSampleRate sr;
	*((double*)&sr) = d;
	if ( ASE_OK != m_pDriver->setSampleRate( sr )  )
		setLastErrorStr( getDriverError().c_str() );
}

double CAsioDrv::getSampleRate()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	ASIOSampleRate sr;
	if ( ASE_OK != m_pDriver->getSampleRate( &sr )  )
	{	setLastErrorStr( getDriverError().c_str() );
		return 0;
	} // end if

	return *((double*)&sr);
}


sqbind::CSqMulti CAsioDrv::getInfo()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return sqbind::CSqMulti();
	} // end if

	sqbind::CSqMulti m;

	// Fill in driver information
	m[ oexT( "driver" ) ][ oexT( "id" ) ].set( sqbind::ToStr( m_nId ) );

	long nVersion = m_pDriver->getDriverVersion();
	m[ oexT( "driver" ) ][ oexT( "version" ) ].set( sqbind::ToStr( m_pDriver->getDriverVersion() ) );
	m[ oexT( "driver" ) ][ oexT( "version_str" ) ].set( sqbind::ToStr( ( nVersion >> 24 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( ( nVersion >> 16 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( ( nVersion >> 8 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( nVersion & 0xff ) );

	char name[ 256 ] = { 0 };
	m_pDriver->getDriverName( name );
	m[ oexT( "driver" ) ][ oexT( "name" ) ].set( oexMbToStrPtr( name ) );

	// Get the driver sample rate
	ASIOSampleRate sr;
	if ( ASE_OK == m_pDriver->getSampleRate( &sr )  )
		m[ oexT( "driver" ) ][ oexT( "sample_rate" ) ].set( sqbind::ToStr( *((double*)&sr) ) );
	else
		m[ oexT( "driver" ) ][ oexT( "err" ) ].set( getDriverError().c_str() );

	// Save the number of available channels
	m[ oexT( "driver" ) ][ oexT( "input_channels" ) ].set( sqbind::ToStr( m_nInputs ) ),
	m[ oexT( "driver" ) ][ oexT( "output_channels" ) ].set( sqbind::ToStr( m_nOutputs ) );

	ASIOChannelInfo	ci;
	for ( long i = 0; i < m_nInputs; i++ )
	{
		ci.channel = i; ci.isInput = ASIOTrue;
		if ( ASE_OK == m_pDriver->getChannelInfo( &ci )  )
		{	sqbind::CSqMulti &r = m[ oexT( "input" ) ][ sqbind::ToStr( i ) ];
			r[ oexT( "channel" ) ].set( sqbind::ToStr( ci.channel ) );
			r[ oexT( "is_input" ) ].set( sqbind::ToStr( ci.isInput ) );
			r[ oexT( "is_active" ) ].set( sqbind::ToStr( ci.isActive ) );
			r[ oexT( "channelGroup" ) ].set( sqbind::ToStr( ci.channelGroup ) );
			r[ oexT( "type" ) ].set( sqbind::ToStr( ci.type ) );
			r[ oexT( "name" ) ].set( sqbind::ToStr( ci.name ? oexMbToStrPtr( ci.name ) : oexT( "" ) ) );
		} // end if
		else
			m[ oexT( "driver" ) ][ oexT( "err") ].set( getDriverError().c_str() );

	} // end for

	for ( long i = 0; i < m_nOutputs; i++ )
	{
		ci.channel = i; ci.isInput = ASIOFalse;
		if ( ASE_OK == m_pDriver->getChannelInfo( &ci )  )
		{	sqbind::CSqMulti &r = m[ oexT( "output" ) ][ sqbind::ToStr( i ) ];
			r[ oexT( "channel" ) ].set( sqbind::ToStr( ci.channel ) );
			r[ oexT( "is_input" ) ].set( sqbind::ToStr( ci.isInput ) );
			r[ oexT( "is_active" ) ].set( sqbind::ToStr( ci.isActive ) );
			r[ oexT( "channelGroup" ) ].set( sqbind::ToStr( ci.channelGroup ) );
			r[ oexT( "type" ) ].set( sqbind::ToStr( ci.type ) );
			r[ oexT( "name" ) ].set( sqbind::ToStr( ci.name ? oexMbToStrPtr( ci.name ) : oexT( "" ) ) );
		} // end if
		else
			m[ oexT( "driver" ) ][ oexT( "err" ) ].set( getDriverError().c_str() );

	} // end for

	// Read in buffer information
	long lMin = 0, lMax = 0, lPref = 0, lGran = 0;
	if ( ASE_OK == m_pDriver->getBufferSize( &lMin, &lMax, &lPref, &lGran )  )
	{	m[ oexT( "buffer" ) ][ oexT( "min" ) ].set( sqbind::ToStr( lMin ) );
		m[ oexT( "buffer" ) ][ oexT( "max" ) ].set( sqbind::ToStr( lMax ) );
		m[ oexT( "buffer" ) ][ oexT( "preferred" ) ].set( sqbind::ToStr( lPref ) );
		m[ oexT( "buffer" ) ][ oexT( "granulatity" ) ].set( sqbind::ToStr( lGran ) );
	} // end if
	else
		m[ oexT( "buffer" ) ][ oexT( "err" ) ].set( getDriverError().c_str() );

	// Get the input and output latencies
	// (input latency is the age of the first sample in the currently returned audio block)
	// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
	long lInputLatency =0, lOutputLatency =0;
	if ( ASE_OK == m_pDriver->getLatencies( &lInputLatency, &lOutputLatency) )
	{	m[ oexT( "latency" ) ][ oexT( "input" ) ].set( sqbind::ToStr( lInputLatency ) );
		m[ oexT( "latency" ) ][ oexT( "output" ) ].set( sqbind::ToStr( lOutputLatency ) );
	}
	else
		m[ oexT( "buffer" ) ][ oexT( "err" ) ].set( getDriverError().c_str() );

	return m;
}

static int CAsioDrv_AsioTypeToSqType( int t )
{
	switch( t )
	{
		default :
			break;

		case ASIOSTInt16LSB :
			return oex::obj::tInt16 & ~oex::obj::eTypeBigEndian;

		case ASIOSTInt16MSB :
			return oex::obj::tInt16 | oex::obj::eTypeBigEndian;

		case ASIOSTInt24LSB :
			return oex::obj::tInt24 & ~oex::obj::eTypeBigEndian;

		case ASIOSTInt24MSB :
			return oex::obj::tInt24 | oex::obj::eTypeBigEndian;

		case ASIOSTInt32LSB :
			return oex::obj::tInt32 & ~oex::obj::eTypeBigEndian;

		case ASIOSTInt32MSB :
			return oex::obj::tInt32 | oex::obj::eTypeBigEndian;

		case ASIOSTFloat32LSB :
			return oex::obj::tFloat32 & ~oex::obj::eTypeBigEndian;

		case ASIOSTFloat32MSB :
			return oex::obj::tFloat32 | oex::obj::eTypeBigEndian;

		case ASIOSTFloat64LSB :
			return oex::obj::tFloat64 & ~oex::obj::eTypeBigEndian;

		case ASIOSTFloat64MSB :
			return oex::obj::tFloat64 | oex::obj::eTypeBigEndian;

	} // end switch

	return 0;
}

int CAsioDrv::CloseChannels()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{//	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	// Be sure it's stopped
	Stop();

		// Drop any open share
	m_share.Destroy();

	// Drop audio buffers
	if ( ASE_OK != m_pDriver->disposeBuffers() )
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "disposeBuffers() failed : " ), getDriverError().c_str() ) ) );

	// Lose buffer memory
	m_bi.Destroy();

	// Lose interleave buffer
	m_buf.Free();

	// Free the slot map
	m_chmap.Destroy();

	// Free instance pointer
	oexAutoLock ll( m_lock );
	if ( ll.IsLocked() && m_pInst == this )
		m_pInst = 0;

	// Reset samples per frame
	m_nSamples = 0;
	m_nSampleType = 0;
	m_nOpenShares = 0;
	m_nOpenChannels = 0;

	return 1;
}

int CAsioDrv::CreateChannelMap( sqbind::CSqMulti *m )
{
	// Reset open channel values
	m_nOpenShares = 0;
	m_nOpenChannels = 0;

	// Allocate channel map
	if ( m_chmap.OexNew( eMaxChannels ).Size() != eMaxChannels )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to allocate channel map : " ), eMaxChannels ) ) );
		return 0;
	} // end if

	// Initialize the channel map
	for ( long i = 0; i < eMaxChannels; i++ )
		m_chmap[ i ] = -1;

	// For each share
	for ( sqbind::CSqMulti::iterator it = m->begin(); m->end() != it; it++ )
		if ( 0 < it->first.length() )
		{
			// Count a share
			m_nOpenShares++;

			// Is it an array?
			if ( !it->second.size() )
			{
				// Get the channel number
				int nCh = oexStrToULong( it->second.c_str(), 10 );

				// If it's valid and the first time we've seen it
				if ( 0 <= nCh && eMaxChannels > nCh && 0 > m_chmap[ nCh ] )
					m_chmap[ nCh ] = 0, m_nOpenChannels++;

			} // end if

			// For each channel in this share
			else for ( sqbind::CSqMulti::iterator ch = it->second.begin(); it->second.end() != ch; ch++ )
			{
				// Get the channel number
				int nCh = oexStrToULong( ch->first.c_str(), 10 );

				// If it's valid and the first time we've seen it
				if ( 0 <= nCh && eMaxChannels > nCh && 0 > m_chmap[ nCh ] )
					m_chmap[ nCh ] = 0, m_nOpenChannels++;

			} // end for

		} // end if

	if ( 0 >= m_nOpenShares )
	{	setLastErrorStr( oexT( "Invalid number of shares" ) );
		return 0;
	} // end if

	if ( 0 >= m_nOpenChannels )
	{	setLastErrorStr( oexT( "Invalid number of channels" ) );
		return 0;
	} // end if

	return 1;
}

int CAsioDrv::OpenChannels( sqbind::CSqMulti *m, int nFlags, int nFmt )
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	// Close any open channels
	CloseChannels();

	// Sanity checks
	if ( !m )
	{	setLastErrorStr( oexT( "Invalid parameters" ) );
		return 0;
	} // end if

	// Lock
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
	{	setLastErrorStr( oexT( "Failed to acquire thread lock" ) );
		return 0;
	} // end if

	// Ensure someone else isn't running
	if ( m_pInst && m_pInst != this )
	{	setLastErrorStr( oexT( "Another instance is already running" ) );
		return 0;
	} // end if

	// Save instance pointer
	m_pInst = this;

	// Create a channel map
	if ( !CreateChannelMap( m ) )
	{	CloseChannels();
		return 0;
	} // end if

	// Allocate memory for buffers
	if ( m_bi.OexNew( m_nOpenChannels ).Size() != m_nOpenChannels )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to allocate channel buffers : " ), m_nOpenChannels ) ) );
		CloseChannels();
		return 0;
	} // end if

	// Get a pointer to the buffer data
	ASIOBufferInfo *pAbi = m_bi.Ptr();
	if ( !pAbi )
	{	setLastErrorStr( oexT( "Bad channel buffer pointer" ) );
		CloseChannels();
		return 0;
	} // end if

	// Open the channels
	long nCh = 0, nType = 0, nSlot = 0;
	for ( long i = 0; i < eMaxChannels; i++ )
		if ( 0 <= m_chmap[ i ] )
		{
			// Reverse map
			m_chmap[ i ] = nCh;

			// Set channel number
			pAbi[ nCh ].channelNum = i;

			// Is this an input?
			pAbi[ nCh ].isInput = ( nFlags & 1 ) ? ASIOFalse : ASIOTrue;

			// Init buffer pointers
			pAbi[ nCh ].buffers[ 0 ] = pAbi[ nCh ].buffers[ 1 ] = 0;

			// Get channel info
			ASIOChannelInfo	ci;
			ci.channel = pAbi[ nCh ].channelNum;
			ci.isInput = pAbi[ nCh ].isInput;
			if ( ASE_OK != m_pDriver->getChannelInfo( &ci )  )
			{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getChannelInfo() failed : " ), getDriverError().c_str() ) ) );
				CloseChannels();
				return 0;
			} // end if

			// Save data type
			if ( !nType )
			{	nType = CAsioDrv_AsioTypeToSqType( ci.type );
				if ( !nType )
				{	setLastErrorStr( oexT( "Invalid sample type" ) );
					CloseChannels();
					return 0;
				} // end if
			} // end if

			// All channels must have the same type
			else if ( nType != CAsioDrv_AsioTypeToSqType( ci.type ) )
			{	setLastErrorStr( oexT( "Audio channels have different data types" ) );
				CloseChannels();
				return 0;
			} // end if

			// Next slot
			if ( ++nCh > m_nOpenChannels )
			{	setLastErrorStr( oexT( "Channel overflow" ) );
				CloseChannels();
				return 0;
			} // end if

		} // end for / if

	// Did we get a valid data type?
	if ( 0 >= nType )
	{	setLastErrorStr( oexT( "Invalid sample type" ) );
		CloseChannels();
		return 0;
	} // end if

	// Save sample type
	m_nSampleType = nType;

	// Ensure all channels were accounted for
	if ( nCh < m_nOpenChannels )
	{	setLastErrorStr( oexT( "Failed to find all channel info" ) );
		CloseChannels();
		return 0;
	} // end if

	long lMin = 0, lMax = 0, lPref = 0, lGran = 0;
	if ( ASE_OK != m_pDriver->getBufferSize( &lMin, &lMax, &lPref, &lGran ) )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getBufferSize() failed : " ), getDriverError().c_str() ) ) );
		CloseChannels();
		return 0;
	} // end if

	// Ensure semi-reasonable size
	if ( 0 >= lPref )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getBufferSize() returned invalid preferred size : " ), lPref ) ) );
		CloseChannels();
		return 0;
	} // end if

	// Save number of samples per frame
	m_nSamples = lPref;

	// Let's try and allocate buffers
	if ( ASE_OK != m_pDriver->createBuffers( pAbi, m_nOpenChannels, lPref, &m_cb ) )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "createBuffers() failed : " ), getDriverError().c_str() ) ) );
		CloseChannels();
		return 0;
	} // end if

	ASIOSampleRate sr;
	if ( ASE_OK != m_pDriver->getSampleRate( &sr )  )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getSampleRate() failed : " ), getDriverError().c_str() ) ) );
		CloseChannels();
		return 0;
	} // end if

	// Number of bytes per channel buffer
	long lBytes = m_nSamples * oex::obj::StaticSize( m_nSampleType );

	// +++ This should be more efficent, but for now interleave in extra buffer

	// If we have more than one channel, we'll need to interleave the data
	if ( 1 < m_nOpenChannels )
		if ( !m_buf.Allocate( m_nOpenChannels * lBytes ) )
		{	setLastErrorStr( oexT( "Failed to allocate interleave buffer" ) );
			CloseChannels();
			return 0;
		} // end if

	// We're using all of it
	m_buf.setUsed( m_nOpenChannels * lBytes );

	// Calculate the amount of space needed for three seconds
	int nBufSize = 3 * oex::obj::StaticSize( nType ) * *((double*)&sr);

	// Allocate memory for buffers
	if ( m_share.OexConstructArray( m_nOpenShares ).Size() != m_nOpenShares )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to allocate memory for share objects: " ), m_nOpenShares ) ) );
		CloseChannels();
		return 0;
	} // end if

	// Create memory shares
	long nS = 0;
	for ( sqbind::CSqMulti::iterator it = m->begin(); m->end() != it; it++ )
		if ( 0 < it->first.length() )
		{
			// Allocate a channel map
			if ( m_share[ nS ].chmap.OexNew( eMaxChannels ).Size() != eMaxChannels )
			{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to allocate memory for channel map: " ), eMaxChannels ) ) );
				CloseChannels();
				return 0;
			} // end if

			// Add each channel to this share
			m_share[ nS ].channels = 0;
			if ( !it->second.size() )
			{
				// Get the channel number
				int nCh = oexStrToULong( it->second.c_str(), 10 );

				// Add channel if it's valid
				if ( 0 <= nCh && eMaxChannels > nCh && eMaxChannels > m_share[ nS ].channels )
					m_share[ nS ].chmap[ m_share[ nS ].channels++ ] = nCh;

			} // end for

			// Add all channels
			else for ( sqbind::CSqMulti::iterator ch = it->second.begin(); it->second.end() != ch; ch++ )
			{
				// Get the channel number
				int nCh = oexStrToULong( ch->first.c_str(), 10 );

				// Add channel if it's valid
				if ( 0 <= nCh && eMaxChannels > nCh && eMaxChannels > m_share[ nS ].channels )
					m_share[ nS ].chmap[ m_share[ nS ].channels++ ] = nCh;

			} // end for

			// Create memory share
			sqbind::stdString sShare = it->first;
			m_share[ nS ].share.setCbId( getCbId() );
			m_share[ nS ].share.setPrefix( getPrefix() );
			
			sqbind::CSqMulti mHeaders;
			mHeaders[ oexT( "channels" ) ].set( sqbind::ToStr( m_share[ nS ].channels ) );
			mHeaders[ oexT( "type" ) ].set( sqbind::ToStr( nType ) );
			mHeaders[ oexT( "type_size" ) ].set( sqbind::ToStr( oex::obj::StaticSize( nType ) ) );
			mHeaders[ oexT( "sample_rate" ) ].set( sqbind::ToStr( *((double*)&sr) ) );
			mHeaders[ oexT( "format" ) ].set( sqbind::ToStr( nFmt ? nFmt : nType ) );

//			if ( !m_share[ nS ].share.Create( sShare, oexT( "" ), nBufSize, m_share[ nS ].channels, 
//											  oex::obj::StaticSize( nType ), *((double*)&sr), nFmt ? nFmt : nType ) )
			if ( !m_share[ nS ].share.Create( sShare, oexT( "" ), nBufSize, 1024, mHeaders.getJSON() ) )
			{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to create memory share : nBufSize = " ), nBufSize,
													oexT( " : nType = " ), nType,
													oexT( " : data size = " ), oex::obj::StaticSize( nType ),
													oexT( " : nFmt = " ), nFmt,
													oexT( " : sShare = " ), sShare.c_str()
													) ) );
				CloseChannels();
				return 0;
			} // end if

			// Count a share
			nS++;

		} // end if

	// Ensure all shares were accounted for
	if ( nS < m_nOpenShares )
	{	setLastErrorStr( oexT( "Some audio shares failed to open" ) );
		CloseChannels();
		return 0;
	} // end if

	return m_nOpenChannels;
}

void CAsioDrv::_onBufferSwitch( long doubleBufferIndex, ASIOBool directProcess )
{
	if ( !CAsioDrv::m_pInst )
		return;
	m_pInst->onBufferSwitch( doubleBufferIndex, directProcess );
}

void CAsioDrv::onBufferSwitch( long doubleBufferIndex, ASIOBool directProcess )
{_STT();

oexSHOW( doubleBufferIndex );
oexSHOW( directProcess );
if ( 1 < doubleBufferIndex )
	return;

	// Get timestamp
	ASIOTime ti; oexZero( ti );
	if(ASE_OK == ASIOGetSamplePosition( &ti.timeInfo.samplePosition, &ti.timeInfo.systemTime) )
		ti.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;

	// Pass on to time callback
	onBufferSwitchTimeInfo( &ti, doubleBufferIndex, directProcess );
}

void CAsioDrv::_onSampleRateDidChange( ASIOSampleRate sRate )
{_STT();
	if ( !CAsioDrv::m_pInst )
		return;
	m_pInst->onSampleRateDidChange( sRate );
}

void CAsioDrv::onSampleRateDidChange( ASIOSampleRate sRate )
{_STT();
oexM();
}

long CAsioDrv::_onAsioMessage( long selector, long value, void* message, double* opt )
{_STT();
	if ( !CAsioDrv::m_pInst )
		return 0L;
	return m_pInst->onAsioMessage( selector, value, message, opt );
}

long CAsioDrv::onAsioMessage( long selector, long value, void* message, double* opt )
{_STT();

	switch( selector )
	{
		default :
			break;

		case kAsioSelectorSupported :
			return ( value == kAsioResetRequest
					|| value == kAsioEngineVersion
					|| value == kAsioResyncRequest
					|| value == kAsioLatenciesChanged
					|| value == kAsioSupportsTimeInfo
					|| value == kAsioSupportsTimeCode
					|| value == kAsioSupportsInputMonitor
					) ? 1L : 0L;

		case kAsioResetRequest :
			m_nSignal = eDriverMsgReset;
			return 1L;

		case kAsioResyncRequest :
			m_nSignal = eDriverMsgResync;
			return 1L;

		case kAsioLatenciesChanged :
			m_nSignal = eDriverMsgResync;
			return 1L;

		case kAsioEngineVersion :
			return 2L;

		case kAsioSupportsTimeInfo :
			return 1L;

		case kAsioSupportsTimeCode :
			return 1L;
	}

	return 0L;
}

#if NATIVE_INT64
	#define ASIO64toDouble(a)  (a)
#else
	const double twoRaisedTo32 = 4294967296.;
	#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif

ASIOTime* CAsioDrv::_onBufferSwitchTimeInfo( ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess )
{	if ( !CAsioDrv::m_pInst )
		return 0L;
	return m_pInst->onBufferSwitchTimeInfo( params, doubleBufferIndex, directProcess );
}

ASIOTime* CAsioDrv::onBufferSwitchTimeInfo( ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess )
{_STT();

#if defined( oexDEBUG )
//	if ( params->timeInfo.flags & kSystemTimeValid )
//		oexSHOW( ASIO64toDouble( params->timeInfo.systemTime ) );
//	if ( params->timeInfo.flags & kSamplePositionValid )
//		oexSHOW( ASIO64toDouble( params->timeInfo.samplePosition ) );
//	if ( params->timeCode.flags & kTcValid )
//		oexSHOW( ASIO64toDouble( params->timeCode.timeCodeSamples ) );
#endif

	// Do we have a valid buffer?
	ASIOBufferInfo *pAbi = m_bi.Ptr();
	if ( !pAbi )
		return 0;

	// Number of bytes in each buffer
	long lSampleSize = oex::obj::StaticSize( m_nSampleType );
	long lBytes = m_nSamples * lSampleSize;
	if ( 0 >= lBytes )
		return 0;

	// For each share
	for( long nSh = 0; nSh < m_share.Size(); nSh++ )
	{
		// Number of channels in this share
		long nChannels = m_share[ nSh ].channels;

		// If it's one, we can just copy the data
		if ( 1 == nChannels )
		{	long nCh = m_share[ nSh ].chmap[ 0 ];
			if ( 0 <= nCh && eMaxChannels > nCh )
			{	long nIdx = m_chmap[ nCh ];
				if ( 0 <= nIdx && nIdx < m_nOpenChannels )
					m_share[ nSh ].share.WritePtr( pAbi[ nIdx ].buffers[ doubleBufferIndex ], lBytes, oexT( "" ), 0, 0 );
			} // end if
		} // end if

		// Channels have to be interleaved :(
		else 
		{
			// For each channel
			for ( long ch = 0; ch < nChannels; ch++ )
			{
				// Convert channel buffer index
				long nCh = m_share[ nSh ].chmap[ ch ];
				if ( 0 <= nCh && eMaxChannels > nCh )
				{	long nIdx = m_chmap[ nCh ];
					if ( 0 <= nIdx && nIdx < m_nOpenChannels )
					{
						switch( lSampleSize )
						{
							case 1 :
							{	unsigned char *src = (unsigned char*)pAbi[ nIdx ].buffers[ doubleBufferIndex ];
								unsigned char *dst = (unsigned char*)m_buf._Ptr(); dst += ch;
								for ( long s = 0; s < m_nSamples; s++, dst += nChannels, src++ )
									*dst = *src;
							} break;

							case 2 :
							{	unsigned short *src = (unsigned short*)pAbi[ nIdx ].buffers[ doubleBufferIndex ];
								unsigned short *dst = (unsigned short*)m_buf._Ptr(); dst += ch;
								for ( long s = 0; s < m_nSamples; s++, dst += nChannels, src++ )
									*dst = *src;
							} break;

							case 4 :
							{	unsigned int *src = (unsigned int*)pAbi[ nIdx ].buffers[ doubleBufferIndex ];
								unsigned int *dst = (unsigned int*)m_buf._Ptr(); dst += ch;
								for ( long s = 0; s < m_nSamples; s++, dst += nChannels, src++ )
									*dst = *src;
							} break;

							case 8 :
							{	unsigned long long *src = (unsigned long long*)pAbi[ nIdx ].buffers[ doubleBufferIndex ];
								unsigned long long *dst = (unsigned long long*)m_buf._Ptr(); dst += ch;
								for ( long s = 0; s < m_nSamples; s++, dst += nChannels, src++ )
									*dst = *src;
							} break;

						} // end switch

					} // end if

				} // end if

			} // end for

#if defined( oexDEBUG )
//			oexSHOW( m_buf.getUsed() );
//			oexEcho( oexBinToAsciiHexStr( m_buf, 0, 16, 16 ).Ptr() );
#endif

			// Write data to share
			m_share[ nSh ].share.Write( &m_buf, oexT( "" ), 0, 0 );

		} // end else

	} // end if

	return 0;
}

int CAsioDrv::Start()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	if ( 0 >= getOpenChannels() )
	{	setLastErrorStr( oexT( "There are no open channels to start" ) );
		return 0;
	} // end if

	// Run the ASIO device
	if ( ASE_OK != m_pDriver->start() )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Start() failed : " ), getDriverError().c_str() ) ) );
		return 0;
	} // end if

	return 1;
}

int CAsioDrv::Stop()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	if ( 0 >= getOpenChannels() )
	{//	setLastErrorStr( oexT( "There are no open channels to stop" ) );
		return 0;
	} // end if

	// Stop the ASIO device
	if ( ASE_OK != m_pDriver->stop() )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Stop() failed : " ), getDriverError().c_str() ) ) );
		return 0;
	} // end if

	return 1;
}

