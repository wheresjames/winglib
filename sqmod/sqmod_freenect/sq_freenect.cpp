// sq_freenect.cpp

#include "stdafx.h"


CSqFreenect::CSqFreenect()
{_STT();

	m_pfc = oexNULL;
	m_pfd = oexNULL;
	m_nInit = -1;
	m_nDev = -1;

}

CSqFreenect::~CSqFreenect()
{_STT();
	Destroy();
}

void CSqFreenect::Destroy()
{_STT();

	CloseDevice();

	if ( 0 <= m_nInit && m_pfc )
		freenect_shutdown( m_pfc );

	m_pfc = oexNULL;
	m_nInit = -1;
}

int CSqFreenect::Init()
{_STT();

	// Out with the old
	Destroy();

	// Initialize USB library
	m_nInit = freenect_init( &m_pfc, NULL );
	if ( 0 > m_nInit )
	{	m_pfc = oexNULL;
		return 0;
	} // end if

	// Set the log function
	freenect_set_log_callback( m_pfc, &CSqFreenect::_LogCallback );

	// Set logging level
#if defined( oexDEBUG )
	setLogLevel( FREENECT_LOG_DEBUG );
#endif

	return 1;
}

void CSqFreenect::setLogLevel( int level )
{_STT();
	if ( !m_pfc )
		return;

	freenect_set_log_level( m_pfc, (freenect_loglevel)level );
}


void CSqFreenect::_LogCallback( freenect_context *dev, freenect_loglevel level, const char *msg )
{_STT();

	if ( !dev )
		return;

	oexEcho( oexMks( level, oexT( " : " ), msg ? msg : oexT( "<NULL>" ) ).Ptr() );

	// hmmmm....
}

void CSqFreenect::LogCallback( freenect_loglevel level, const char *msg )
{_STT();

	m_sErrorLog += oexMks( level, oexT( " : " ), msg ? msg : oexT( "<NULL>" ) ).Ptr();

}


int CSqFreenect::getNumDevices()
{_STT();
	if ( !m_pfc )
		return 0;

	return freenect_num_devices( m_pfc );
}

int CSqFreenect::CloseDevice()
{_STT();
	if ( 0 <= m_nDev && m_pfd )
		freenect_set_user( m_pfd, 0 ),	
		freenect_close_device( m_pfd );

	m_pfd = oexNULL;
	m_nDev = -1;

	return 0;
}

int CSqFreenect::OpenDevice( int i )
{_STT();
	if ( !m_pfc )
		return 0;

	// Attempt to open the device
	m_nDev = freenect_open_device( m_pfc, &m_pfd, i );
	if ( 0 > m_nDev || !m_pfd )
		return 0;

	// Save device index
	m_nDev = i;

	// Set user data
	freenect_set_user( m_pfd, this );
	
	// Set callbacks
	freenect_set_depth_callback( m_pfd, CSqFreenect::_OnDepth );
	freenect_set_rgb_callback( m_pfd, CSqFreenect::_OnRGB );
	
	return 1;
}

sqbind::CSqMulti CSqFreenect::getDeviceList()
{_STT();

	if ( 0 > m_nInit )
		return sqbind::CSqMulti();

	return sqbind::CSqMulti();
}

int CSqFreenect::Update()
{_STT();

	if ( !m_pfc )
		return 0;

	return freenect_process_events( m_pfc );
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

}

void CSqFreenect::_OnRGB( freenect_device *dev, freenect_pixel *rgb, uint32_t timestamp )
{_STT();

	if ( !dev )
		return;
	
	CSqFreenect *p = (CSqFreenect*)freenect_get_user( dev );
	if ( p ) 
		p->OnRGB( rgb, timestamp );
}

void CSqFreenect::OnRGB( freenect_pixel *rgb, uint32_t timestamp )
{_STT();

}


void CSqFreenect::setLED( int nState )
{_STT();

	if ( !m_pfd )
		return;

	freenect_set_led( m_pfd, (freenect_led_options)nState );
}

void CSqFreenect::setTilt( double dAngle )
{_STT();

	if ( !m_pfd )
		return;

	freenect_set_tilt_degs( m_pfd, dAngle );
}

void CSqFreenect::StartRGB()
{_STT();

	if ( !m_pfd )
		return;

	freenect_start_rgb( m_pfd );
}

void CSqFreenect::StopRGB()
{_STT();

	if ( !m_pfd )
		return;

	freenect_stop_rgb( m_pfd );
}

void CSqFreenect::StartDepth()
{_STT();

	if ( !m_pfd )
		return;

	freenect_start_depth( m_pfd );
}

void CSqFreenect::StopDepth()
{_STT();

	if ( !m_pfd )
		return;

	freenect_stop_depth( m_pfd );
}

sqbind::CSq3dVector CSqFreenect::getRawAccel()
{_STT();

	if ( !m_pfd )
		return sqbind::CSq3dVector();
		
	int16_t x = 0, y = 0, z = 0;
	freenect_get_raw_accel( m_pfd, &x, &y, &z );
	
	return sqbind::CSq3dVector( (SQInteger)x, (SQInteger)y, (SQInteger)z );
}

sqbind::CSq3dVectord CSqFreenect::getMksAccel()
{_STT();

	if ( !m_pfd )
		return sqbind::CSq3dVectord();

	double x = 0, y = 0, z = 0;
	freenect_get_mks_accel( m_pfd, &x, &y, &z );
	
	return sqbind::CSq3dVectord( x, y, z );
}

