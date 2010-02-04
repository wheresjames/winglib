
// sq_fftw.cpp

#include "stdafx.h"

CSqFftw::CSqFftw()
{_STT();
	m_plan_valid = 0;
	m_in = oexNULL;
	m_out = oexNULL;
	m_size = 0;
	m_samples = 0;
}

CSqFftw::~CSqFftw()
{_STT();
	Destroy();
}

void CSqFftw::Destroy()
{_STT();
	// Lose plan
	if ( m_plan_valid )
		fftw_destroy_plan( m_plan );

	if ( m_in )
		fftw_free( m_in );

	if ( m_out )
		fftw_free( m_out );

	m_plan_valid = 0;
	m_in = oexNULL;
	m_out = oexNULL;
	m_size = 0;
	m_samples = 0;
}

int CSqFftw::Allocate( int samples )
{_STT();
	Destroy();

	// Must be a power of 2
	samples = oex::cmn::PrevPower2( samples );
	if ( !samples )
		return 0;

	// What will the size of the array be?
	int size = sizeof( fftw_complex ) * samples; 
	if ( !size )
		return 0;

	// Allocate input buffer
	m_in = (fftw_complex*) fftw_malloc( size );
	if ( !m_in )
	{	Destroy();
		return 0;
	} // end if

	// Allocate output buffer
	m_out = (fftw_complex*) fftw_malloc( size );
	if ( !m_out )
	{	Destroy();
		return 0;
	} // end if

	// Save buffer info
	m_size = size;
	m_samples = samples;

	return 1;
}

int CSqFftw::Plan( int samples, int reverse, int measure )
{_STT();
	// Allocate space
	if ( !Allocate( samples ) )
		return 0;

	// Get the plan
	m_plan = 
		fftw_plan_dft_1d( m_samples, m_in, m_out, 
						  reverse ? FFTW_BACKWARD : FFTW_FORWARD, 
						  measure ? FFTW_ESTIMATE : FFTW_MEASURE );

	m_plan_valid = 1;

	return 1;
}

int CSqFftw::Execute()
{_STT();
	if ( !m_plan_valid )
		return 0;

	fftw_execute( m_plan );

	return 1;
}

