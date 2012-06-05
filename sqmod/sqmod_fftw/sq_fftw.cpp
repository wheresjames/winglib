
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

int CSqFftw::ReadInput( int nType, int nInterval, sqbind::CSqBinary *bin )
{
	if ( !m_plan_valid || !bin || !bin->getUsed() )
		return 0;

	int nSize = oex::obj::StaticSize( nType );
	if ( 0 >= nSize )
		return 0;

	if ( nSize > nInterval )
		nInterval = nSize;

	// How many samples to read?
	int nSamples = bin->getUsed() / nInterval;
	if ( 0 >= nSamples )
		return 0;

	// Don't overrun the buffer
	if ( nSamples > m_samples )
		nSamples = m_samples;

#define _FFTW_SET_TYPE( t, c ) \
	case t : \
		for ( int i = 0; i < nSamples; i++ ) \
			setInput( i, *(c*)bin->Ptr( i * nInterval ), 0 ); \
		break;

	switch( nType )
	{	_FFTW_SET_TYPE( oex::obj::tInt8, oex::oexCHAR );
		_FFTW_SET_TYPE( oex::obj::tUInt8, oex::oexUCHAR );
		_FFTW_SET_TYPE( oex::obj::tInt16, oex::oexSHORT );
		_FFTW_SET_TYPE( oex::obj::tUInt16, oex::oexUSHORT );
		_FFTW_SET_TYPE( oex::obj::tInt32, oex::oexINT32 );
		_FFTW_SET_TYPE( oex::obj::tUInt32, oex::oexUINT32 );
		_FFTW_SET_TYPE( oex::obj::tInt64, oex::oexINT64 );
		_FFTW_SET_TYPE( oex::obj::tUInt64, oex::oexUINT64 );
		_FFTW_SET_TYPE( oex::obj::tFloat, oex::oexFLOAT );
		_FFTW_SET_TYPE( oex::obj::tDouble, oex::oexDOUBLE );
	} // end switch

	return nSamples;
}

int CSqFftw::ReadOutputMagnitudes( int nMax, int nType, int nInterval, sqbind::CSqBinary *bin )
{
	if ( !m_plan_valid || !bin )
		return 0;

	int nSize = oex::obj::StaticSize( nType );
	if ( 0 >= nSize )
		return 0;

	if ( nSize > nInterval )
		nInterval = nSize;

	// Bounds check
	if ( 0 >= nMax || m_samples < nMax )
		nMax = m_samples;

	// How much space do we need?
	int nBytes = nMax * nInterval;
	if ( 0 >= nBytes )
		return 0;

	// Ensure space
	if ( bin->Size() < (unsigned int)nBytes )
		if ( !bin->Allocate( nBytes ) )
			return 0;

#define _FFTW_GET_TYPE( t, c ) \
	case t : \
		for ( int i = 0; i < nMax; i++ ) \
			*(c*)bin->Ptr( i * nInterval ) = getOutputMag( i ); \
		break;

	switch( nType )
	{	_FFTW_GET_TYPE( oex::obj::tInt8, oex::oexCHAR );
		_FFTW_GET_TYPE( oex::obj::tUInt8, oex::oexUCHAR );
		_FFTW_GET_TYPE( oex::obj::tInt16, oex::oexSHORT );
		_FFTW_GET_TYPE( oex::obj::tUInt16, oex::oexUSHORT );
		_FFTW_GET_TYPE( oex::obj::tInt32, oex::oexINT32 );
		_FFTW_GET_TYPE( oex::obj::tUInt32, oex::oexUINT32 );
		_FFTW_GET_TYPE( oex::obj::tInt64, oex::oexINT64 );
		_FFTW_GET_TYPE( oex::obj::tUInt64, oex::oexUINT64 );
		_FFTW_GET_TYPE( oex::obj::tFloat, oex::oexFLOAT );
		_FFTW_GET_TYPE( oex::obj::tDouble, oex::oexDOUBLE );
	} // end switch

	// Set the number of bytes used
	bin->setUsed( nBytes );

	return 1;
}

int CSqFftw::CalcMagnitudes( int nInType, int nInInterval, sqbind::CSqBinary *in, int nOutType, int nOutInterval, sqbind::CSqBinary *out )
{
	if ( !m_plan_valid || !in || !out )
		return 0;

	int nInSize = oex::obj::StaticSize( nInType );
	if ( 0 >= nInSize )
		return 0;

	if ( nInSize > nInInterval )
		nInInterval = nInSize;

	int nInSamples = in->getUsed() / nInInterval;
	int nInSets = nInSamples / m_samples;
	if ( !nInSets )
		return 0;

	int nOutSize = oex::obj::StaticSize( nOutType );
	if ( 0 >= nOutSize )
		return 0;

	if ( nOutSize > nOutInterval )
		nOutInterval = nOutSize;

	// How much space do we need for output?
	int nOutSamples = m_samples / 2;
	int nOutOffset = out->getUsed();
	int nOutBytes = nOutOffset + nInSets * nOutSamples * nOutInterval;
	if ( out->Size() < nOutBytes )
		if ( !out->Resize( nOutBytes ) )
			return 0;

	sqbind::CSqBinary view;
	for ( int i = 0; i < nInSets; i++ )
	{
		// Get view of input data
		if ( !in->Sub( &view, i * m_samples * nInInterval, m_samples * nInInterval ) )
			return 0;

		// Read in the data
		if ( !ReadInput( nInType, nInInterval, &view ) )
			return 0;

		Execute();

		// Get view of output data
		if ( !out->Sub( &view, nOutOffset + i * nOutSamples * nOutInterval, nOutSamples * nOutInterval ) )
			return 0;

		// Read the output data
		if ( !ReadOutputMagnitudes( nOutSamples, nOutType, nOutInterval, &view ) )
			return 0;

	} // end for

	// Set the output size
	out->setUsed( nOutBytes );

	// How many input bytes did we use?
	return nInSets * m_samples * nInInterval;
}

