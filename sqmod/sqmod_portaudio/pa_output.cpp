// pa_output.cpp

#include "stdafx.h"


CPaOutput::CPaOutput()
{_STT();

	m_errInit = -1;
	m_pdi = oexNULL;
	m_stream = oexNULL;
	m_errLast = paNoError;
	m_bBlocking = 0;

	// Initialize portaudio
	Init();
}

CPaOutput::~CPaOutput()
{_STT();

	Destroy();

	if ( paNoError == m_errInit )
	{	Pa_Terminate();
		m_errInit = -1;
	} // end if
}

void CPaOutput::Destroy()
{_STT();

	// Close the stream
	if ( m_stream )
		Pa_StopStream( m_stream ),
		Pa_CloseStream( m_stream );

	m_pdi = oexNULL;  
	m_stream = oexNULL;
	m_bBlocking = 0;
}

int CPaOutput::Init()
{_STT();

	// Initialize
	m_errInit = Pa_Initialize();
	if ( paNoError != m_errInit )
		return 0;

	return 1;
}

int CPaOutput::getDeviceCount()
{_STT();
	return Pa_GetDeviceCount();
}

int CPaOutput::getDefaultOutputDevice()
{_STT();
	return Pa_GetDefaultOutputDevice();
}

int CPaOutput::getDeviceInfo( int nDev, sqbind::CSqMulti *pInf )
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

int CPaOutput::_PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags, void *userData )
{	CPaOutput *p = (CPaOutput*)userData;
	if ( !p ) return 0;
	return p->PaStreamCallback( input, output, frameCount, timeInfo, statusFlags );
}

int CPaOutput::PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags )
{
	// Anything to do?
	if ( 0 >= frameCount )
		return paAbort;

	// Do we have the minimum required number of bytes?
	int bytes = frameCount * getFrameBytes();
	if ( bytes > m_buf.GetMaxRead() )
		return paContinue;

	// Copy data from the ring buffer
	m_buf.Read( output, bytes );

	return paContinue; // paComplete paAbort
}

int CPaOutput::getFormatBytes( int nFmt )
{_STT();

	switch( nFmt )
	{
		default : break;
		case paFloat32 : return 4;
		case paInt32 : return 4;
		case paInt24 : return 3;
		case paInt16 : return 2;
		case paInt8 : return 1;

	} // end switch

	return 1;
}


int CPaOutput::Open( int bBlocking, int nDev, int nChannels, int nFormat, double dLatency, double dSRate, int fpb )
{_STT();

	// Lose old stream
	Destroy();

	PaStreamParameters psp;
	oexZero( psp );

	psp.device = nDev;
	psp.channelCount = nChannels;
	psp.sampleFormat = nFormat;
	psp.suggestedLatency = dLatency;

	m_bBlocking = bBlocking;
	m_nFrameBytes = getFormatBytes( nFormat ) * nChannels;

	// Attempt to open output stream

	if ( bBlocking )
		m_errLast = Pa_OpenStream( &m_stream, 0, &psp, dSRate, fpb, 0, 0, this );
	else
		m_errLast = Pa_OpenStream( &m_stream, 0, &psp, dSRate, fpb, 0, &_PaStreamCallback, this );

	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaOutput::Start()
{_STT();

	if ( !m_stream )
		return 0;

	// Attempt to start the output
	m_errLast = Pa_StartStream( m_stream );
	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaOutput::Stop()
{_STT();

	if ( !m_stream )
		return 0;

	// Attempt to stop the output
	m_errLast = Pa_StopStream( m_stream );
	if ( paNoError != m_errLast )
		return 0;

	return 1;
}

int CPaOutput::Write( sqbind::CSqBinary *data, int frames )
{_STT();

	// Sanity checks
	if ( !m_stream || !data || !data->getUsed() || !frames )
		return 0;

	if ( m_bBlocking )
	{
		m_errLast = Pa_WriteStream( m_stream, data->Ptr(), frames );
		if ( paNoError != m_errLast )
			return 0;

	} // end if

	else

		// Stuff in the ring buffer
		m_buf.Write( data->Ptr(), data->getUsed() );

	return 1;
}
