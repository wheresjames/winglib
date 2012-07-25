
#include "stdafx.h"

#include "asiolist.h"

/*
struct SThunk32
{
	unsigned char	m_stk[ 10 ];
	unsigned int	m_mov;
	unsigned int	m_this;
	unsigned char	m_jmp;
	unsigned int	m_dst;

	static void* Bind( void *pFun, void *pThis )
	{
		SThunk32 *p = (SThunk32*)malloc( sizeof( SThunk32 ) );
		if ( !p )
			return 0;

		// Enable execute permissions
		long pgsz = sysconf( _SC_PAGE_SIZE );
		if ( 0 < pgsz && !( pgsz | ( pgsz - 1 ) ) )
		{	long sz = pgsz;
			while ( sz && sz < sizeof( SThunk32 ) )
				sz++;
			if ( 0 < sz )
				mprotect( (void*)( p & ~pgsz), sz, PROT_READ | PROT_WRITE | PROT_EXEC )
		} // end if

		p->m_stk[ 0 ] = 0x83;	//	sub esp, 4
		p->m_stk[ 1 ] = 0xec;
		p->m_stk[ 2 ] = 0x04;
		p->m_stk[ 3 ] = 0x8b;	//	mov eax,[esp + 4]
		p->m_stk[ 4 ] = 0x44;
		p->m_stk[ 5 ] = 0x24;
		p->m_stk[ 6 ] = 0x04;
		p->m_stk[ 7 ] = 0x89;	//	mov [esp], eax
		p->m_stk[ 8 ] = 0x04;
		p->m_stk[ 9 ] = 0x24;

		p->m_mov = 0x042444C7;	//	mov   dword   ptr   [esp+0x4], 

		p->m_this = oexPtrToUInt( pThis );

		p->m_jmp = 0xe9;		//	jmp

		p->m_dst = oexPtrToUInt( pFun ) - ( oexPtrToUInt( p ) + sizeof( SThunk32 ) );

		cacheflush( p, sizeof( SThunk32 ), ICACHE );

		return p;
	}
} __attribute__((packed));
*/

struct SThunk32
{
#pragma pack ( push, 1 )
	unsigned short	m_lea;
	unsigned int	m_this;
	unsigned char	m_mov;
	unsigned int	m_dst;
	unsigned short	m_jmp;
#pragma pack ( pop )

	static void* Bind( void *pFun, void *pThis )
	{
		SThunk32 *p = (SThunk32*)VirtualAlloc( 0, sizeof( SThunk32 ), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		if ( !p )
			return 0;

		p->m_lea = 0x0d8d;
		p->m_this = oexPtrToUInt( p );
		p->m_mov = 0xb8;
		p->m_dst = oexPtrToUInt( pFun );
		p->m_jmp = 0xe0ff;

		FlushInstructionCache( GetCurrentProcess(), p, sizeof( SThunk32 ) );

		return p;
	}

};

// +++ ????
// oexSTATIC_ASSERT( sizeof( ASIOSampleRate ) == sizeof( double ) );

// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CAsioDrv, CAsioDrv )

	SQBIND_MEMBER_FUNCTION( CAsioDrv, Destroy )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setLastErrorStr )
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
//	SQBIND_MEMBER_FUNCTION( CAsioDrv,  )
//	SQBIND_MEMBER_FUNCTION( CAsioDrv,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CAsioDrv );

void CAsioDrv::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CAsioDrv );
}

CAsioDrv::CAsioDrv()
{_STT();

	m_nId = -1;
	m_pDriver = 0;
	m_nSignal = 0;
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
	*m_adi.errorMessage = 0;
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
			m[ sqbind::oex2std( oexMks( i ) ) ][ oexT( "name" ) ].set( name );
		else
			i = nDrivers;

	return m;
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
	if ( err || !m_pDriver )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "asioOpenDriver() failed : " ),
										  nId, oexT( " : " ), err ) ) );
		return 0;
	} // end if

	*m_adi.name = 0;
	*m_adi.errorMessage = 0;
	m_adi.driverVersion = 0;
	if ( !m_pDriver->init( m_adi.sysRef ) )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "ASIOInit() failed : " ),
										  nId, oexT( " : " ), err,
										  oexT( " : " ), m_adi.errorMessage ) ) );
		Destroy();
		return 0;
	} // end if

	// onBufferSwitch
	typedef void (*pfn_bufferSwitch)( long doubleBufferIndex, ASIOBool directProcess );
	m_cb.bufferSwitch 
		= m_cbOnBufferSwitch
			.CDecl2This< pfn_bufferSwitch >( this, &CAsioDrv::onBufferSwitch,
											 sizeof( long ) + sizeof( ASIOBool ) );

	// onSampleRateDidChange
	typedef void (*pfn_sampleRateDidChange)( ASIOSampleRate sRate );
	m_cb.sampleRateDidChange 
		= m_cbOnSampleRateDidChange
				.CDecl2This< pfn_sampleRateDidChange >( this, &CAsioDrv::onSampleRateDidChange,
														sizeof( long ) + sizeof( ASIOBool ) );


	// onAsioMessage
	typedef long (*pfn_asioMessage)( long selector, long value, void* message, double* opt );
	m_cb.asioMessage 
		= m_cbOnAsioMessage
			.CDecl2This< pfn_asioMessage >( this, &CAsioDrv::onAsioMessage,
											sizeof( long ) + sizeof( long ) 
											+ sizeof( void*) + sizeof( double* ) );

	// onBufferSwitchTimeInfo
	typedef ASIOTime* (*pfn_bufferSwitchTimeInfo) (ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
	m_cb.bufferSwitchTimeInfo 
		= m_cbOnBufferSwitchTimeInfo
				.CDecl2This< pfn_bufferSwitchTimeInfo >( this, &CAsioDrv::onBufferSwitchTimeInfo,
														 sizeof( ASIOTime* ) + sizeof( long )
														 + sizeof( ASIOBool ) );

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
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( m_adi.errorMessage );
	} // end if
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
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( m_adi.errorMessage );
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
	m[ "driver"][ "id" ].set( sqbind::ToStr( m_nId ) );

	long nVersion = m_pDriver->getDriverVersion();
	m[ "driver"][ "version" ].set( sqbind::ToStr( m_pDriver->getDriverVersion() ) );
	m[ "driver"][ "version_str" ].set( sqbind::ToStr( ( nVersion >> 24 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( ( nVersion >> 16 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( ( nVersion >> 8 ) & 0xff ) + oexT( "." )
									   + sqbind::ToStr( nVersion & 0xff ) );

	char name[ 256 ] = { 0 };
	m_pDriver->getDriverName( name );
	m[ "driver" ][ "name" ].set( name );

	// Get the driver sample rate
	ASIOSampleRate sr;
	if ( ASE_OK == m_pDriver->getSampleRate( &sr )  )
		m[ "driver" ][ "sample_rate" ].set( sqbind::ToStr( *((double*)&sr) ) );
	else
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		m[ "driver" ][ "err" ].set( m_adi.errorMessage );
	} // end if

	// Get the number of available channels
	long nInputs = 0, nOutputs = 0;
	if ( ASE_OK == m_pDriver->getChannels( &nInputs, &nOutputs ) )
		m[ "driver" ][ "input_channels" ].set( sqbind::ToStr( nInputs ) ),
		m[ "driver" ][ "output_channels" ].set( sqbind::ToStr( nOutputs ) );
	else
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		m[ "driver" ][ "err" ].set( m_adi.errorMessage );
	} // end if

	ASIOChannelInfo	ci;
	for ( long i = 0; i < nInputs; i++ )
	{
		ci.channel = i; ci.isInput = ASIOTrue;
		if ( ASE_OK == m_pDriver->getChannelInfo( &ci )  )
		{	sqbind::CSqMulti &r = m[ "input" ][ sqbind::ToStr( i ) ];
			r[ "channel" ].set( sqbind::ToStr( ci.channel ) );
			r[ "is_input" ].set( sqbind::ToStr( ci.isInput ) );
			r[ "is_active" ].set( sqbind::ToStr( ci.isActive ) );
			r[ "channelGroup" ].set( sqbind::ToStr( ci.channelGroup ) );
			r[ "type" ].set( sqbind::ToStr( ci.type ) );
			r[ "name" ].set( sqbind::ToStr( ci.name ? ci.name : "" ) );
		} // end if
		else
		{	m_pDriver->getErrorMessage( m_adi.errorMessage );
			m[ "driver" ][ "err" ].set( m_adi.errorMessage );
		} // end if

	} // end for

	// Read in buffer information
	long lMin = 0, lMax = 0, lPref = 0, lGran = 0;
	if ( ASE_OK == m_pDriver->getBufferSize( &lMin, &lMax, &lPref, &lGran )  )
	{	m[ "buffer" ][ "min" ].set( sqbind::ToStr( lMin ) );
		m[ "buffer" ][ "max" ].set( sqbind::ToStr( lMax ) );
		m[ "buffer" ][ "preferred" ].set( sqbind::ToStr( lPref ) );
		m[ "buffer" ][ "granulatity" ].set( sqbind::ToStr( lGran ) );
	} // end if
	else
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		m[ "buffer" ][ "err" ].set( m_adi.errorMessage );
	} // end if

	// Get the input and output latencies
	// (input latency is the age of the first sample in the currently returned audio block)
	// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
	long lInputLatency =0, lOutputLatency =0;
	if ( ASE_OK == m_pDriver->getLatencies( &lInputLatency, &lOutputLatency) )
	{
		m[ "latency" ][ "input" ].set( sqbind::ToStr( lInputLatency ) );
		m[ "latency" ][ "output" ].set( sqbind::ToStr( lOutputLatency ) );
	}
	else
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		m[ "buffer" ][ "err" ].set( m_adi.errorMessage );
	} // end if

	return m;
}

static int CAsioDrv_AsioTypeToSqType( int t )
{
	switch( t )
	{
		default :
			break;

		case ASIOSTInt16LSB :
		case ASIOSTInt16MSB :
			return oex::obj::tInt16;
			break;

		case ASIOSTInt24LSB :
		case ASIOSTInt24MSB :
			return oex::obj::tInt24;
			break;

		case ASIOSTInt32LSB :
		case ASIOSTInt32MSB :
			return oex::obj::tInt32;
			break;

		case ASIOSTFloat32MSB :
			return oex::obj::tFloat32;

		case ASIOSTFloat64MSB :
			return oex::obj::tFloat64;

	} // end switch

	return 0;
}

int CAsioDrv::CloseChannels()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	// Drop any open share
	m_share.Destroy();

	// Drop audio buffers
	if ( ASE_OK != m_pDriver->disposeBuffers() )
		m_pDriver->getErrorMessage( m_adi.errorMessage ),
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "disposeBuffers() failed : " ), m_adi.errorMessage ) ) );

	// Lose buffer memory
	m_bi.Destroy();

	return 1;
}

int CAsioDrv::OpenChannels( sqbind::CSqMulti *m, const sqbind::stdString &sShare, int nFlags, int nFmt )
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	// Close any open channels
	CloseChannels();

	// Sanity checks
	if ( !m || 0 >= sShare.length() )
	{	setLastErrorStr( oexT( "Invalid parameters" ) );
		return 0;
	} // end if

	// How many channels
	int nChannels = m->size();
	if ( 0 >= nChannels )
	{	setLastErrorStr( oexT( "Invalid number of channels" ) );
		return 0;
	} // end if

	// Allocate memory for buffers
	if ( m_bi.OexNew( nChannels ).Size() != nChannels )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to allocate memory : " ), nChannels ) ) );
		return 0;
	} // end if

	// Get a pointer to the buffer data
	ASIOBufferInfo *pAbi = m_bi.Ptr();
	if ( !pAbi )
	{	setLastErrorStr( oexT( "Bad pointer" ) );
		return 0;
	} // end if

	int i = 0, nType = 0;
	for ( sqbind::CSqMulti::iterator it = m->begin(); m->end() != it; it++ )
	{
		// Set channel number
		pAbi[ i ].channelNum = oexStrToULong( it->first.c_str(), 10 );

		// Is this an input?
		pAbi[ i ].isInput = ( nFlags & 1 ) ? ASIOFalse : ASIOTrue;

		// Init buffer pointers
		pAbi[ i ].buffers[ 0 ] = pAbi[ i ].buffers[ 1 ] = 0;

		// Get channel info
		ASIOChannelInfo	ci;
		ci.channel = pAbi[ i ].channelNum; ci.isInput = pAbi[ i ].isInput;
		if ( ASE_OK != m_pDriver->getChannelInfo( &ci )  )
		{	m_pDriver->getErrorMessage( m_adi.errorMessage );
			setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getChannelInfo() failed : " ), m_adi.errorMessage ) ) );
			return 0;
		} // end if

		// Save data type
		if ( !nType )
			nType = CAsioDrv_AsioTypeToSqType( ci.type );
		else if ( nType != CAsioDrv_AsioTypeToSqType( ci.type ) )
		{	setLastErrorStr( oexT( "Audio channels have different data types" ) );
			return 0;
		} // end if

		// Next slot
		if ( ++i > nChannels )
		{	setLastErrorStr( oexT( "Channel overflow" ) );
			return 0;
		} // end if

	} // end for

	// Ensure all channels were accounted for
	if ( i < nChannels )
	{	setLastErrorStr( oexT( "Failed to find all channel info" ) );
		return 0;
	} // end if

	long lMin = 0, lMax = 0, lPref = 0, lGran = 0;
	if ( ASE_OK != m_pDriver->getBufferSize( &lMin, &lMax, &lPref, &lGran ) )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getBufferSize() failed : " ), m_adi.errorMessage ) ) );
		return 0;
	} // end if

	// Ensure semi-reasonable size
	if ( 0 >= lPref )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getBufferSize() returned invalid preferred size : " ), lPref ) ) );
		return 0;
	} // end if

	// Let's try and allocate buffers
	if ( ASE_OK != m_pDriver->createBuffers( pAbi, nChannels, lPref, &m_cb ) )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "createBuffers() failed : " ), m_adi.errorMessage ) ) );
		return 0;
	} // end if

	ASIOSampleRate sr;
	if ( ASE_OK != m_pDriver->getSampleRate( &sr )  )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "getSampleRate() failed : " ), m_adi.errorMessage ) ) );
		return 0;
	} // end if

	// Calculate the amount of space needed for three seconds
	int nBufSize = 3 * oex::obj::StaticSize( nType ) * *((double*)&sr);

	// Create memory share
	if ( !m_share.Create( sShare, oexT( "" ), nBufSize, nChannels, nType, *((double*)&sr), nFmt ) )
	{	setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Failed to create memory share : nBufSize = " ), nBufSize,
											oexT( " : nType = " ), nType,
											oexT( " : nFmt = " ), nFmt
											) ) );
		return 0;
	} // end if

	return nChannels;
}

void CAsioDrv::onBufferSwitch( long doubleBufferIndex, ASIOBool directProcess )
{_STT();

	// Get timestamp
	ASIOTime ti; oexZero( ti );
	if(ASE_OK == ASIOGetSamplePosition( &ti.timeInfo.samplePosition, &ti.timeInfo.systemTime) )
		ti.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;

	// Pass on to time callback
	onBufferSwitchTimeInfo( &ti, doubleBufferIndex, directProcess );
}

void CAsioDrv::onSampleRateDidChange( ASIOSampleRate sRate )
{_STT();
oexM();
}

long CAsioDrv::onAsioMessage( long selector, long value, void* message, double* opt )
{//_STT();
oexM();

	switch( selector )
	{
		default :
			break;

		case kAsioSelectorSupported:
			return ( value == kAsioResetRequest
					|| value == kAsioEngineVersion
					|| value == kAsioResyncRequest
					|| value == kAsioLatenciesChanged
					|| value == kAsioSupportsTimeInfo
					|| value == kAsioSupportsTimeCode
					|| value == kAsioSupportsInputMonitor
					) ? 1L : 0L;

		case kAsioResetRequest:
			m_nSignal = eDriverMsgReset;
			return 1L;

		case kAsioResyncRequest:
			m_nSignal = eDriverMsgResync;
			return 1L;

		case kAsioLatenciesChanged:
			m_nSignal = eDriverMsgResync;
			return 1L;

		case kAsioEngineVersion:
			return 2L;

		case kAsioSupportsTimeInfo:
			return 1L;

		case kAsioSupportsTimeCode:
			return 1L;
	}

	return 0L;
}

ASIOTime* CAsioDrv::onBufferSwitchTimeInfo( ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess )
{_STT();
oexM();
	return params;
}

int CAsioDrv::Start()
{_STT();

	// Ensure we have a driver loaded
	if ( !isDriver() )
	{	setLastErrorStr( oexT( "Driver is not loaded" ) );
		return 0;
	} // end if

	// Run the ASIO device
	if ( ASE_OK != m_pDriver->start() )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Start() failed : " ), m_adi.errorMessage ) ) );
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

	// Stop the ASIO device
	if ( ASE_OK != m_pDriver->stop() )
	{	m_pDriver->getErrorMessage( m_adi.errorMessage );
		setLastErrorStr( sqbind::oex2std( oexMks( oexT( "Stop() failed : " ), m_adi.errorMessage ) ) );
		return 0;
	} // end if

	return 1;
}

