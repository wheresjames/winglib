// lv_rtsp_client.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"


CLvRtspClient::CVideoSink::CVideoSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{
}

CLvRtspClient::CVideoSink::~CVideoSink()
{
	m_buf.Free();
}

Boolean CLvRtspClient::CVideoSink::continuePlaying()
{
	if ( !fSource )
		return False;

	if ( !m_buf.Size() )
		m_buf.Obj().OexNew( eDefaultBufferSize );

	fSource->getNextFrame( m_buf._Ptr(), m_buf.Size(), _afterGettingFrame, this, onSourceClosure, this );

	return True;
}

void CLvRtspClient::CVideoSink::_afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  									struct timeval presentationTime, unsigned durationInMicroseconds )
{
//	oexEcho( oexMks( "Rx: ", frameSize, ", ", numTruncatedBytes, ", ", durationInMicroseconds ).Ptr() );

	CVideoSink* pVideoSink = (CVideoSink*)clientData;
	if ( pVideoSink )
		pVideoSink->afterGettingFrame( clientData, frameSize, numTruncatedBytes,
									   presentationTime, durationInMicroseconds );
}

void CLvRtspClient::CVideoSink::afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  								   struct timeval presentationTime, unsigned durationInMicroseconds )
{
	if ( !fSource )
		return;

	// How much data was used this time
	m_buf.setUsed( frameSize );

	// Signal that a new frame is ready
	m_sigFrame.Signal();

	// Wait for frame to get used
	m_sigFrame.GetResetEvent().Wait( 8000 );

//    onSourceClosure( this );

	continuePlaying();
}

int CLvRtspClient::CVideoSink::LockFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{
	// Check to see if a new frame is ready
	if ( m_sigFrame.Wait( 0 ) )
		return 0;

	if ( dat )
		*dat = m_buf;

	return 1;
}

int CLvRtspClient::CVideoSink::UnlockFrame()
{
	// Unlock the buffer
	m_sigFrame.Reset();

	return 1;
}



CLvRtspClient::CLvRtspClient()
{
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspClient = oexNULL;
	m_pSession = oexNULL;
	m_pVs = oexNULL;
	m_nEnd = 0;
}

void CLvRtspClient::Destroy()
{
	// Tell thread loop to exit
	m_nEnd = 1;

	Stop();

	m_nEnd = 0;
	m_sUrl = oexT( "" );
	m_mParams.clear();
}

void CLvRtspClient::ThreadDestroy()
{
	if ( m_pRtspClient && m_pSession )
		m_pRtspClient->teardownMediaSession( *m_pSession );

	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspClient = oexNULL;
	m_pSession = oexNULL;
	m_pVs = oexNULL;
}

int CLvRtspClient::Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m )
{
	Destroy();

	m_sUrl = sUrl;
	if ( m )
		m_mParams = *m;

	Start();

	return 1;
}

int CLvRtspClient::ThreadOpen( const sqbind::stdString &sUrl, sqbind::CSqMulti *m )
{
	// Lose old container
	ThreadDestroy();

	// Did we get a valid string?
	if ( !sUrl.length() )
		return 0;

	// Setup usage environment
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	if ( !scheduler )
	{	oexERROR( 0, oexT( "BasicTaskScheduler::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	m_pEnv = BasicUsageEnvironment::createNew( *scheduler );
	if ( !m_pEnv )
	{	oexERROR( 0, oexT( "BasicUsageEnvironment::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

#if defined( OEX_DEBUG )
int nVerbosity = 1;
#else
int nVerbosity = 0;
#endif

	// Create rtsp client
	m_pRtspClient = RTSPClient::createNew( *m_pEnv, nVerbosity, oexT( "CLvRtspClient" ), 0 );
	if ( !m_pRtspClient )
	{	oexERROR( 0, oexT( "RTSPClient::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	char *pOptions = m_pRtspClient->sendOptionsCmd( sUrl.c_str(), 0, 0 );
	if ( !pOptions )
	{	oexERROR( 0, oexT( "sendOptionsCmd() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Ditch the options
	delete [] pOptions;
	pOptions = oexNULL;

//	char *pSdp = m_pRtspClient->sendOptions( sUrl.c_str(), username, password );
	char *pSdp = m_pRtspClient->describeURL( sUrl.c_str() );
	if ( !pSdp )
	{	oexERROR( 0, oexT( "describeURL() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Create session
	m_pSession = MediaSession::createNew( *m_pEnv, pSdp );

	// Ditch the SDP description
	delete [] pSdp;
	pSdp = oexNULL;

	if ( !m_pSession )
	{	oexERROR( 0, oexT( "MediaSession::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Find a video stream
	MediaSubsessionIterator iter( *m_pSession );
	MediaSubsession *pss;
	while ( 0 != ( pss = iter.next() ) && strncmp( "video", pss->mediumName(), 5 ) )
//		oexEcho( oexMks( pss->mediumName(), " - ", pss->codecName() ).Ptr() )
		;

	if ( !pss )
	{	oexERROR( 0, oexT( "video not found in stream" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate( -1 ) )
	{	oexERROR( 0, oexT( "initiate() video stream failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	oexERROR( 0, oexT( "RTP source is null" ) );
		ThreadDestroy();
		return 0;
	} // end if

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	int sn = pss->rtpSource()->RTPgs()->socketNum();
	setReceiveBufferTo( *m_pEnv, sn, 1000000 );

	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
	{	oexERROR( 0, oexT( "setupMediaSubsession() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	m_pVs = new CVideoSink( *m_pEnv );
	if ( !m_pVs )
	{	oexERROR( 0, oexT( "CVideoSink::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	if ( !m_pVs->startPlaying( *pss->rtpSource(), 0, 0 ) )
	{	oexERROR( 0, oexT( "CVideoSink::startPlaying() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	m_pRtspClient->playMediaSession( *m_pSession, 0, 0, 1.f );

/*
	int width = 640;
	int height = 480;
	int fps = 30;

	AVIFileSink *pAfs = AVIFileSink::createNew( *m_pEnv, *m_pSession, "rtsp_test.avi",
												100000, width, height, fps, False );
	if ( !pAfs )
	{	oexERROR( 0, oexT( "AVIFileSink::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	if ( !pAfs->startPlaying( NULL, NULL ) )
	{	oexERROR( 0, oexT( "AVIFileSink::startPlaying() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	m_pRtspClient->playMediaSession( *m_pSession, 0, 0, 1.f );
*/
	return 1;
}

int CLvRtspClient::LockFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{
	if ( !m_pVs )
		return 0;

	return m_pVs->LockFrame( dat, m );
}

int CLvRtspClient::UnlockFrame()
{
	if ( !m_pVs )
		return 0;

	return m_pVs->UnlockFrame();
}

oex::oexBOOL CLvRtspClient::InitThread( oex::oexPVOID x_pData )
{
	if ( !ThreadOpen( m_sUrl, &m_mParams ) )
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspClient::DoThread( oex::oexPVOID x_pData )
{
	// Run the thing
	if ( !m_pEnv )
		return oex::oexFALSE;

	m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	return oex::oexTRUE;
}

oex::oexINT CLvRtspClient::EndThread( oex::oexPVOID x_pData )
{
	ThreadDestroy();

	return 0;
}


