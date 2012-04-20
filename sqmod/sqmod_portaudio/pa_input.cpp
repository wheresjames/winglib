// pa_input.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CPaInput, CPaInput )

	SQBIND_MEMBER_FUNCTION( CPaInput, Init )
	SQBIND_MEMBER_FUNCTION( CPaInput, Destroy )
	SQBIND_MEMBER_FUNCTION( CPaInput, Open )
	SQBIND_MEMBER_FUNCTION( CPaInput, Start )
	SQBIND_MEMBER_FUNCTION( CPaInput, Stop )
	SQBIND_MEMBER_FUNCTION( CPaInput, Read )

	SQBIND_MEMBER_FUNCTION( CPaInput, getInitError )
	SQBIND_MEMBER_FUNCTION( CPaInput, getLastError )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDefaultInputDevice )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDeviceCount )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDeviceInfo )
	SQBIND_MEMBER_FUNCTION( CPaInput, getBufferedBytes )
	SQBIND_MEMBER_FUNCTION( CPaInput, getFrameBytes )
	SQBIND_MEMBER_FUNCTION( CPaInput, isOpen )
	
//	SQBIND_GLOBALCONST( paFloat32 )
//	SQBIND_GLOBALCONST( paInt16 )
//	SQBIND_GLOBALCONST( paInt32 )
//	SQBIND_GLOBALCONST( paInt24 )
//	SQBIND_GLOBALCONST( paInt8 )
//	SQBIND_GLOBALCONST( paUInt8 )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CPaInput );

void CPaInput::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CPaInput );
}

CPaInput::CPaInput()
{_STT();

	m_errInit = -1;
	m_pdi = oexNULL;
	m_stream = oexNULL;
	m_errLast = paNoError;
	m_bBlocking = 0;
	m_nFrameBlockSize = 0;

	// Initialize portaudio
	Init();
}

CPaInput::~CPaInput()
{_STT();

	Destroy();

	if ( paNoError == m_errInit )
	{	Pa_Terminate();
		m_errInit = -1;
	} // end if
}

void CPaInput::Destroy()
{_STT();

	// Close the stream
	if ( m_stream )
		Pa_StopStream( m_stream ),
		Pa_CloseStream( m_stream );

	m_buf.Destroy();
	m_pdi = oexNULL;
	m_stream = oexNULL;
	m_bBlocking = 0;
	m_nFrameBlockSize = 0;
}

int CPaInput::Init()
{_STT();

	// Initialize
	m_errInit = Pa_Initialize();
	if ( paNoError != m_errInit )
		return 0;

	return 1;
}

int CPaInput::getDeviceCount()
{_STT();
	return Pa_GetDeviceCount();
}

int CPaInput::getDefaultInputDevice()
{_STT();
	return Pa_GetDefaultInputDevice();
}

int CPaInput::getDeviceInfo( int nDev, sqbind::CSqMulti *pInf )
{_STT();

	// Read output device information
    m_pdi = Pa_GetDeviceInfo( nDev );
	if ( !m_pdi )
		return 0;

	if ( !pInf )
		return 1;

	(*pInf)[ oexT( "name" ) ].set( m_pdi->name );
	(*pInf)[ oexT( "ver" ) ].set( oexMks( m_pdi->structVersion ).Ptr() );
	(*pInf)[ oexT( "max_input_channels" ) ].set( oexMks( m_pdi->maxInputChannels ).Ptr() );
	(*pInf)[ oexT( "max_output_channels" ) ].set( oexMks( m_pdi->maxOutputChannels ).Ptr() );
	(*pInf)[ oexT( "default_sample_rate" ) ].set( oexMks( m_pdi->defaultSampleRate ).Ptr() );
	(*pInf)[ oexT( "default_low_input_latency" ) ].set( oexMks( m_pdi->defaultLowInputLatency ).Ptr() );
	(*pInf)[ oexT( "default_low_output_latency" ) ].set( oexMks( m_pdi->defaultLowOutputLatency ).Ptr() );
	(*pInf)[ oexT( "default_high_input_latency" ) ].set( oexMks( m_pdi->defaultHighInputLatency ).Ptr() );
	(*pInf)[ oexT( "default_high_output_latency" ) ].set( oexMks( m_pdi->defaultHighOutputLatency ).Ptr() );

	return 1;
}

int CPaInput::_PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags, void *userData )
{	CPaInput *p = (CPaInput*)userData;
	if ( !p ) return 0;
	return p->PaStreamCallback( input, output, frameCount, timeInfo, statusFlags );
}

int CPaInput::PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags )
{
	// Anything to do?
	if ( 0 >= frameCount )
		return paAbort;

	// Do we have the minimum required number of bytes?
	int bytes = frameCount * getFrameBytes();
	if ( 0 >= bytes )
		return paAbort;

	// Stuff the data into the ring buffer
	m_buf.Write( input, bytes );

	return paContinue;
}

int CPaInput::getFormatBytes( int nFmt )
{_STT();

	int nSize = Pa_GetSampleSize( nFmt );
	if ( paSampleFormatNotSupported == nSize )
		return 0;
	return nSize;
}


int CPaInput::Open( int bBlocking, int nDev, int nChannels, int nFormat, double dLatency, double dSRate, int fpb )
{_STT();

	// Lose old stream
	Destroy();

	PaStreamParameters psp;
	oexZero( psp );

	if ( 0 > fpb )
		fpb = 1024;

	psp.device = nDev;
	psp.channelCount = nChannels;
	psp.suggestedLatency = dLatency;
	
#	define CNVTYPE( t, v ) case oex::obj::t : psp.sampleFormat = v; break;
	switch( nFormat )
	{	CNVTYPE( tInt8, paInt8 );
		CNVTYPE( tUInt8, paUInt8 );
		CNVTYPE( tInt16, paInt16 );
		CNVTYPE( tInt24, paInt24 );
		CNVTYPE( tInt32, paInt32 );
		CNVTYPE( tFloat, paFloat32 );
		default : psp.sampleFormat = 0; break;
	} // end switch

	m_bBlocking = bBlocking;
	m_nFrameBlockSize = fpb;
	m_nFrameBytes = getFormatBytes( nFormat ) * nChannels;

	// Attempt to open output stream

	if ( bBlocking )
		m_errLast = Pa_OpenStream( &m_stream, &psp, 0, dSRate, fpb, 0, 0, this );
	else
		m_errLast = Pa_OpenStream( &m_stream, &psp, 0, dSRate, fpb, 0, &_PaStreamCallback, this );

	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaInput::Start()
{_STT();

	if ( !m_stream )
		return 0;

	// Attempt to start the output
	m_errLast = Pa_StartStream( m_stream );
	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaInput::Stop()
{_STT();

	if ( !m_stream )
		return 0;

	// Attempt to stop the output
	m_errLast = Pa_StopStream( m_stream );
	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaInput::Read( sqbind::CSqBinary *data, int frames )
{_STT();

	// Sanity checks
	if ( !m_stream || !data )
		return 0;

	// Use number of bytes available if blocking
	if ( m_bBlocking && 0 >= frames )
		frames = Pa_GetStreamReadAvailable( m_stream );

	// How many bytes to read?
	unsigned long bytes = frames * getFrameBytes();
	if ( !m_bBlocking && 0 >= bytes )
		bytes = m_buf.GetMaxRead();

	// Ensure space
	unsigned long total = bytes + data->getUsed();
	if ( data->Size() < total )
		if ( !data->Resize( total ) )
			return data->getUsed();

	if ( m_bBlocking )
	{
		// Must specify an amount for blocking read
		if ( 0 >= bytes )
			return data->getUsed();

		// Read from stream
		m_errLast = Pa_ReadStream( m_stream, (void*)data->Ptr( data->getUsed() ), frames );
		if ( paNoError == m_errLast )
			data->setUsed( total );

	} // end if

	else
	{
		oex::CCircBuf::t_size uRead = 0;

		// Read the string from the buffer
		m_buf.Read( (oex::oexPVOID)data->Ptr( data->getUsed() ), bytes, &uRead );

		// Bounds check bytes read
		if ( uRead < 0 ) 
			uRead = 0;
		else if ( uRead > bytes ) 
			uRead = bytes;

		// Set new buffer size
		data->setUsed( data->getUsed() + uRead );

	} // end else

	return data->getUsed();
}
