
// sq_fftw.cpp

#include "stdafx.h"

CSqFftw::CSqFftw()
{
	m_plan_valid = 0;
}

CSqFftw::~CSqFftw()
{
	Destroy();
}

void CSqFftw::Destroy()
{
	// Lose plan
	if ( m_plan_valid )
	{	m_plan_valid = 0;
		fftw_destroy_plan( m_plan );
	} // end if
}

int CSqFftw::Plan( sqbind::CSqBinary *in, sqbind::CSqBinary *out, int reverse, int measure )
{
	// Verify buffer integrity
	if ( !in || !in->getUsed() || !out || !out->getUsed() )
		return 0;

	// How many samples
	long nSamples = in->getUsed() / sizeof( fftw_complex );
	if ( !nSamples )
		return 0;

	// Reference the buffers
	m_in.Share( in );
	m_out.Share( out );

	// Get the plan
	m_plan = 
		fftw_plan_dft_1d( nSamples, (fftw_complex*)m_in.Ptr(), (fftw_complex*)m_out.Ptr(), 
						  reverse ? FFTW_BACKWARD : FFTW_FORWARD, measure ? FFTW_ESTIMATE : FFTW_MEASURE );

	m_plan_valid = 1;

	return 1;
}

int CSqFftw::Execute()
{
	if ( !m_plan_valid )
		return 0;

	fftw_execute( m_plan );

	return 1;
}


