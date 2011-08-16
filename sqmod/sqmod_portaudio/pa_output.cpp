// pa_output.cpp

#include "stdafx.h"


CPaOutput::CPaOutput()
{_STT();

	m_errInit = -1;
	m_pdi = oexNULL;
	m_stream = oexNULL;
	m_errLast = paNoError;
	m_bBlocking = 0;
	m_iWTs = 0;
	m_iRTs = 0;
	m_nTsBytes = 0;
	m_nBlockSize = 0;

	// Enable glitch detection by default
	m_bGlitchDetection = 1;
	m_tsGlitch = 0;
	m_tsLast = 0;
	
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

	m_buf.Destroy();
	m_pdi = oexNULL;
	m_stream = oexNULL;
	m_bBlocking = 0;
	m_iWTs = 0;
	m_iRTs = 0;
	m_nTsBytes = 0;
	m_nBlockSize = 0;

	// Lose timestamps
	for( unsigned int i = 0; i < oexSizeOfArray( m_ts ); i++ )
		m_ts[ i ].nFrames = 0;

	m_tsGlitch = 0;
	m_tsLast = 0;
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
	if ( bytes > (int)m_buf.GetMaxRead() )
	{
		// Play what's left
		bytes = m_buf.GetMaxRead();

		// Forget what's left
//		m_buf.Empty();

//		return paComplete;

	} // end if

	// Copy data from the ring buffer
	m_buf.Read( output, bytes );

	if ( m_nBlockSize < bytes )	
		m_nBlockSize = bytes;
	
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

	if ( 0 > fpb )
		fpb = 1024;

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

	// How did it go?
	if ( paNoError != m_errLast )
	{	m_stream = 0;
		return 0;
	} // end if

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

SQInteger CPaOutput::getTs()
{_STT();

	if ( !m_stream )
		return 0;

	// Just use last timestamp if buffer is empty
	if ( m_iRTs == m_iWTs )
		return m_tsLast;

	int fb = getFrameBytes();
	if ( 0 >= fb ) fb = 1;
	int i = m_iRTs, t = 0, b = getBufferedBytes() / fb;

	// How many total bytes in the ts buffers
	oex::oexINT64 max = m_ts[ i ].ts;
	while ( i != m_iWTs )
	{	if ( max < m_ts[ i ].ts ) 
			max = m_ts[ i ].ts;
		t += m_ts[ i ].nFrames;
		if ( ++i >= eMaxTimestamps )
			i = 0;
	} // end while

	// Pick off used buffers
	while ( m_iRTs != m_iWTs && t > b )
	{	t -= m_ts[ m_iRTs ].nFrames;
		m_ts[ m_iRTs ].ts = 0;
		m_ts[ m_iRTs ].nFrames = 0;
		if ( ++m_iRTs >= eMaxTimestamps )
			m_iRTs = 0;
	} // end while	

	// Calculate timestamp
	oex::oexINT64 min = m_ts[ m_iRTs ].ts, ts = min;
	if ( min && b && b > t )
		ts = min + ( ( max - min ) * ( b - t ) / b );

	// This should be our timestamp
	return ts;
}

int CPaOutput::WriteTs( sqbind::CSqBinary *data, int frames, SQInteger ts )
{_STT();

	// Compensate for reverse timestamp glitches
	if ( m_bGlitchDetection )
		if ( m_tsLast > ts )
			m_tsGlitch += ( m_tsLast - ts );

	// Remember the last timestamp
	m_tsLast = ts;

	// Save timestamps
	m_ts[ m_iWTs ].nFrames = frames;
	m_ts[ m_iWTs ].ts = ts + m_tsGlitch;
	if ( ++m_iWTs >= eMaxTimestamps )
		m_iWTs = 0;		

	return Write( data, frames );
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
