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
		return 0;

	// Set logging level
	freenect_set_log_level( m_pfc, FREENECT_LOG_DEBUG );

	return 1;
}

int CSqFreenect::CloseDevice()
{
	if ( 0 <= m_nDev && m_pfd )
		freenect_close_device( m_pfd );

	m_pfd = oexNULL;
	m_nDev = -1;

	return 0;
}

int CSqFreenect::OpenDevice( int i )
{
	if ( 0 > m_nInit )
		return 0;

	// Attempt to open the device
	m_nDev = freenect_open_device( m_pfc, &m_pfd, i );
	if ( 0 > m_nDev )
		return 0;

	m_nDev = i;

	return 1;
}

sqbind::CSqMulti CSqFreenect::getDeviceList()
{_STT();

	if ( 0 > m_nInit )
		return sqbind::CSqMulti();

	return sqbind::CSqMulti();
}

