// sq_freenect.cpp

#include "stdafx.h"

#include "freenect_internal.h"

CSqFreenect::CSqFreenect()
{_STT();

	m_pfc = oexNULL;
	m_pfd = oexNULL;
	m_nInit = -1;
	m_nNumDevices = -1;

	// No device
	m_nDev[ 0 ] = m_nDev[ 1 ] = -1;

	// Default formats
	m_nVideoFmt[ 0 ] = m_nVideoFmt[ 1 ] = -1;
	m_nDepthFmt[ 0 ] = m_nDepthFmt[ 1 ] = -1;

	// Next frame buffer
	m_nVideoIdx = 0;
	m_nDepthIdx = 0;

	// Initialize states
	m_nLEDState[ 0 ] = m_nLEDState[ 1 ] = 0;
	m_dTiltState[ 0 ] = m_dTiltState[ 1 ] = 0;

}

CSqFreenect::~CSqFreenect()
{_STT();
	Destroy();
}

void CSqFreenect::Destroy()
{_STT();

	// Stop the thread
	Stop();

	// Lose buffers
	for ( int i = 0; i < eMaxBuffers; i++ )
		m_binVideo[ i ].Free(),
		m_binDepth[ i ].Free();

	m_nNumDevices = -1;
	m_nDev[ 0 ] = m_nDev[ 1 ] = -1;

	m_nLEDState[ 0 ] = m_nLEDState[ 1 ] = -1;
	m_dTiltState[ 0 ] = m_dTiltState[ 1 ] = -1;

	m_nVideoFmt[ 0 ] = m_nVideoFmt[ 1 ] = -1;
	m_nDepthFmt[ 0 ] = m_nDepthFmt[ 1 ] = -1;
}

int CSqFreenect::Init()
{_STT();

	// Out with the old
	Destroy();

	//==============================================================
	// Create video buffers

	sqbind::CSqBinary::t_size lSize = getVideoWidth() * getVideoHeight() * 4;
	if ( !lSize )
		return 0;

	// Allocate space for video buffers
	for ( int i = 0; i < eMaxBuffers; i++ )
		if ( lSize > m_binVideo[ i ].Size() && !m_binVideo[ i ].Allocate( lSize ) )
			return 0;
		else
			m_binVideo[ i ].setUsed( lSize );

	//==============================================================
	// Create depth buffers

	lSize = getDepthWidth() * getDepthHeight() * 4;
	if ( !lSize )
		return 0;

	// Allocate space for video buffers
	for ( int i = 0; i < eMaxBuffers; i++ )
		if ( lSize > m_binDepth[ i ].Size() && !m_binDepth[ i ].Allocate( lSize ) )
			return 0;
		else
			m_binDepth[ i ].setUsed( lSize );

	// Start the thread
	Start();

	return 1;
}

void CSqFreenect::_LogCallback( freenect_context *dev, freenect_loglevel level, const char *msg )
{_STT();

	if ( !dev )
		return;

//	oexEcho( oexMks( level, oexT( " : " ), msg ? msg : oexT( "<NULL>" ) ).Ptr() );

	// hmmmm....
}

void CSqFreenect::LogCallback( freenect_loglevel level, const char *msg )
{_STT();

//	m_sErrorLog += oexMks( level, oexT( " : " ), msg ? msg : oexT( "<NULL>" ) ).Ptr();

}

void CSqFreenect::_OnDepth( freenect_device *dev, void *depth, uint32_t timestamp )
{_STT();

	if ( !dev )
		return;
	
	CSqFreenect *p = (CSqFreenect*)freenect_get_user( dev );
	if ( p ) 
		p->OnDepth( depth, timestamp );
}

void CSqFreenect::OnDepth( void *depth, uint32_t timestamp )
{_STT();

	// We got a buffer
	if ( ++m_nDepthIdx >= eMaxBuffers )
		m_nDepthIdx = 0;

	// Switch to next buffer
	int next = m_nDepthIdx + 1;
	if ( next >= eMaxBuffers ) next = 0;
	freenect_set_depth_buffer( m_pfd, m_binDepth[ next ]._Ptr() );
}

void CSqFreenect::_OnVideo( freenect_device *dev, void *rgb, uint32_t timestamp )
{_STT();

	if ( !dev )
		return;
	
	CSqFreenect *p = (CSqFreenect*)freenect_get_user( dev );
	if ( p ) 
		p->OnVideo( rgb, timestamp );
}

void CSqFreenect::OnVideo( void *rgb, uint32_t timestamp )
{_STT();

	// We got a buffer
	if ( ++m_nVideoIdx >= eMaxBuffers )
		m_nVideoIdx = 0;

	// Set next buffer
	int next = m_nVideoIdx + 1;
	if ( next >= eMaxBuffers ) next = 0;
	freenect_set_video_buffer( m_pfd, m_binVideo[ next ]._Ptr() );

}

sqbind::CSq3dVector CSqFreenect::getRawAccel()
{_STT();

	if ( !m_pfd )
		return sqbind::CSq3dVector();
		
	int16_t x = 0, y = 0, z = 0;
//	freenect_get_raw_accel( m_pfd, &x, &y, &z );
	
	return sqbind::CSq3dVector( (SQInteger)x, (SQInteger)y, (SQInteger)z );
}

sqbind::CSq3dVectord CSqFreenect::getMksAccel()
{_STT();

	if ( !m_pfd )
		return sqbind::CSq3dVectord();

	double x = 0, y = 0, z = 0;
	freenect_get_mks_accel( freenect_get_tilt_state( m_pfd ), &x, &y, &z );
	
	return sqbind::CSq3dVectord( x, y, z );
}


int CSqFreenect::ThreadInitVideo()
{_STT();

	if ( !m_pfd )
		return 0;

	// Set the video buffer
	m_nVideoIdx = 0;
	freenect_set_video_buffer( m_pfd, m_binVideo[ 1 ]._Ptr() );

	// Set the video callback function
	freenect_set_video_callback( m_pfd, CSqFreenect::_OnVideo );

	return 1;
}

int CSqFreenect::ThreadInitDepth()
{_STT();

	if ( !m_pfd )
		return 0;

	// Set the video buffer
	m_nDepthIdx = 0;
	freenect_set_depth_buffer( m_pfd, m_binDepth[ 1 ]._Ptr() );

	// Set the video callback function
	freenect_set_depth_callback( m_pfd, CSqFreenect::_OnDepth );

	return 1;
}

int CSqFreenect::ThreadUpdate()
{_STT();

	if ( !m_pfc )
		return 0;

	return freenect_process_events( m_pfc );
}

oex::oexBOOL CSqFreenect::InitThread( oex::oexPVOID x_pData )
{_STT();

	// Initialize USB library
	int nInit = freenect_init( &m_pfc, oexNULL );
	if ( 0 > nInit )
	{	m_pfc = oexNULL;
		ThreadDestroy();
		return oex::oexFALSE;
	} // end if

	// Set the log level
//	freenect_set_log_level( m_pfc, (freenect_loglevel)FREENECT_LOG_ERROR );
//	freenect_set_log_level( m_pfc, (freenect_loglevel)FREENECT_LOG_DEBUG );
//	libusb_set_debug( m_pfc->usb.ctx, 0 );

	// Set the log function
	freenect_set_log_callback( m_pfc, &CSqFreenect::_LogCallback );

	// Save the number of devices
	m_nNumDevices = freenect_num_devices( m_pfc );

	// We're initialized
	m_nInit = 0;

	return oex::oexTRUE;
}

int CSqFreenect::ThreadOpenDevice( int i )
{_STT();

	if ( !m_pfc )
		return 0;

	// Lose any previous device
	ThreadCloseDevice();

	// Attempt to open the device
	int nDev = freenect_open_device( m_pfc, &m_pfd, i );
	if ( 0 > nDev || !m_pfd )
		return 0;

	// Set user data
	freenect_set_user( m_pfd, this );
	
	if ( !ThreadInitVideo() )
		return 0;	

	if ( !ThreadInitDepth() )
		return 0;

	// Init tilt state
	m_dTiltState[ 0 ] = m_dTiltState[ 1 ] =
		freenect_get_tilt_degs( freenect_get_tilt_state( m_pfd ) );

	// Save device index
	m_nDev[ 1 ] = i;

	return 1;
}

int CSqFreenect::ThreadCloseDevice()
{_STT();

	// Close any open device
	if ( 0 <= m_nDev && m_pfd )
		freenect_set_user( m_pfd, 0 ),	
		freenect_close_device( m_pfd );

	m_pfd = oexNULL;
	m_nDev[ 1 ] = -1;

	return 0;
}

oex::oexBOOL CSqFreenect::DoThread( oex::oexPVOID x_pData )
{_STT();
	
	// Until the thread exits
	while ( oex::os::CResource::waitSuccess != GetStopEvent().Wait( 0 ) )
	{
		// Must have context
		if ( !m_pfc )
			return oex::oexFALSE;

		// Process freenect events
		if ( 0 <= m_nVideoFmt[ 1 ] || 0 <= m_nDepthFmt[ 1 ] )
			freenect_process_events( m_pfc );

		// Do we need to open a device?
		if ( 0 <= m_nDev[ 0 ] && m_nDev[ 0 ] != m_nDev[ 1 ] )
			ThreadOpenDevice( m_nDev[ 0 ] );

		// Do we need to close a device?
		else if ( 0 > m_nDev[ 0 ] && 0 <= m_nDev[ 1 ] )
			ThreadCloseDevice();

		// Punt if no open device
		if ( 0 > m_nDev[ 1 ] )
			return oex::oexTRUE;

		// Start / stop video
		if ( 0 <= m_nVideoFmt[ 0 ] && m_nVideoFmt[ 0 ] != m_nVideoFmt[ 1 ] )
			freenect_set_video_format( m_pfd, (freenect_video_format)m_nVideoFmt[ 0 ] ),
			freenect_start_video( m_pfd );
		else if ( 0 > m_nVideoFmt[ 0 ] )
			freenect_stop_video( m_pfd );
		m_nVideoFmt[ 1 ] = m_nVideoFmt[ 0 ];

		// Start /  stop depth
		if ( 0 <= m_nDepthFmt[ 0 ] && m_nDepthFmt[ 0 ] != m_nDepthFmt[ 1 ] )
			freenect_set_depth_format( m_pfd, (freenect_depth_format)m_nDepthFmt[ 0 ] ),
			freenect_start_depth( m_pfd );
		else if ( 0 > m_nDepthFmt[ 0 ] )
			freenect_stop_depth( m_pfd );
		m_nDepthFmt[ 1 ] = m_nDepthFmt[ 0 ];

		// Update tilt state
		if ( m_dTiltState[ 0 ] != m_dTiltState[ 1 ] )
		{	m_dTiltState[ 1 ] = m_dTiltState[ 0 ];
			freenect_set_tilt_degs( m_pfd, m_dTiltState[ 0 ] );		
		} // end if

		// Update tilt state
		if ( m_nLEDState[ 0 ] != m_nLEDState[ 1 ] )
		{	m_nLEDState[ 1 ] = m_nLEDState[ 0 ];
			freenect_set_led( m_pfd, (freenect_led_options)m_nLEDState[ 0 ] );
		} // end if

	} // end while

	return oex::oexTRUE;
}

oex::oexINT CSqFreenect::EndThread( oex::oexPVOID x_pData )
{_STT();

	// Close out
	ThreadDestroy();

	return 0;
}

void CSqFreenect::ThreadDestroy()
{_STT();

	ThreadCloseDevice();

	// Close freenect library
	if ( 0 <= m_nInit && m_pfc )
		freenect_shutdown( m_pfc );
	m_pfc = oexNULL;
	m_nInit = -1;
}

