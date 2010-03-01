// lv_rtsp_client.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"


CLvRtspClient::CVideoSink::CVideoSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{_STT();
}

CLvRtspClient::CVideoSink::~CVideoSink()
{_STT();
	m_buf.Free();
}

Boolean CLvRtspClient::CVideoSink::continuePlaying()
{_STT();
	if ( !fSource )
		return False;

	if ( !m_buf.Size() )
		m_buf.Mem().Mem().OexNew( eDefaultBufferSize );

	fSource->getNextFrame( (unsigned char*)m_buf._Ptr(), m_buf.Size(), _afterGettingFrame, this, onSourceClosure, this );

	return True;
}

void CLvRtspClient::CVideoSink::_afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  									struct timeval presentationTime, unsigned durationInMicroseconds )
{_STT();
//	oexEcho( oexMks( "Rx: ", frameSize, ", ", numTruncatedBytes, ", ", durationInMicroseconds ).Ptr() );

	CVideoSink* pVideoSink = (CVideoSink*)clientData;
	if ( pVideoSink )
		pVideoSink->afterGettingFrame( clientData, frameSize, numTruncatedBytes,
									   presentationTime, durationInMicroseconds );
}

void CLvRtspClient::CVideoSink::afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  								   struct timeval presentationTime, unsigned durationInMicroseconds )
{_STT();
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
{_STT();
	// Check to see if a new frame is ready
	if ( m_sigFrame.Wait( 0 ) )
		return 0;

	if ( dat )
		*dat = m_buf;

	return 1;
}

int CLvRtspClient::CVideoSink::UnlockFrame()
{_STT();
	// Unlock the buffer
	m_sigFrame.Reset();

	return 1;
}


CLvRtspClient::CAudioSink::CAudioSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{_STT();
}

CLvRtspClient::CAudioSink::~CAudioSink()
{_STT();
	m_buf.Free();
}

Boolean CLvRtspClient::CAudioSink::continuePlaying()
{_STT();
	if ( !fSource )
		return False;

	if ( !m_buf.Size() )
		m_buf.Mem().Mem().OexNew( eDefaultBufferSize );

	fSource->getNextFrame( (unsigned char*)m_buf._Ptr(), m_buf.Size(), _afterGettingFrame, this, onSourceClosure, this );

	return True;
}

void CLvRtspClient::CAudioSink::_afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  									struct timeval presentationTime, unsigned durationInMicroseconds )
{_STT();
//	oexEcho( oexMks( "Rx: ", frameSize, ", ", numTruncatedBytes, ", ", durationInMicroseconds ).Ptr() );

	CAudioSink* pAudioSink = (CAudioSink*)clientData;
	if ( pAudioSink )
		pAudioSink->afterGettingFrame( clientData, frameSize, numTruncatedBytes,
									   presentationTime, durationInMicroseconds );
}

void CLvRtspClient::CAudioSink::afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  								   struct timeval presentationTime, unsigned durationInMicroseconds )
{_STT();
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

int CLvRtspClient::CAudioSink::LockFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	// Check to see if a new frame is ready
	if ( m_sigFrame.Wait( 0 ) )
		return 0;

	if ( dat )
		*dat = m_buf;

	return 1;
}

int CLvRtspClient::CAudioSink::UnlockFrame()
{_STT();
	// Unlock the buffer
	m_sigFrame.Reset();

	return 1;
}



CLvRtspClient::CLvRtspClient()
{_STT();
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspClient = oexNULL;
	m_pSession = oexNULL;
	m_pVs = oexNULL;
	m_pAs = oexNULL;
	m_nEnd = 0;
	m_bVideo = 1;
	m_bAudio = 1;
}

void CLvRtspClient::Destroy()
{_STT();
	// Tell thread loop to exit
	m_nEnd = 1;

	Stop();

	m_nEnd = 0;
	m_sUrl = oexT( "" );
	m_mParams.clear();
}

void CLvRtspClient::ThreadDestroy()
{_STT();
	if ( m_pRtspClient && m_pSession )
		m_pRtspClient->teardownMediaSession( *m_pSession );

	if ( m_pRtspClient )
		m_pRtspClient->close( m_pSession );

	if ( m_pEnv )
		m_pEnv->reclaim();

	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspClient = oexNULL;
	m_pSession = oexNULL;
	m_pVs = oexNULL;
	m_pAs = oexNULL;
	m_sVideoCodec = oexT( "" );
}

int CLvRtspClient::Open( const sqbind::stdString &sUrl, int bVideo, int bAudio, sqbind::CSqMulti *m )
{_STT();
	Destroy();

	m_sUrl = sUrl;
	if ( m )
		m_mParams = *m;

	m_bVideo = bVideo;
	m_bAudio = bAudio;

	Start();

	return 1;
}

int CLvRtspClient::ThreadOpen( const sqbind::stdString &sUrl, int bVideo, int bAudio, sqbind::CSqMulti *m )
{_STT();
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

#if defined( oexDEBUG )
int nVerbosity = 1;
#else
int nVerbosity = 0;
#endif

	// Create rtsp client
	m_pRtspClient = RTSPClient::createNew( *m_pEnv, nVerbosity, "CLvRtspClient", 0 );
	if ( !m_pRtspClient )
	{	oexERROR( 0, oexT( "RTSPClient::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	char *pOptions = m_pRtspClient->sendOptionsCmd( oexStrToMbPtr( sUrl.c_str() ), 0, 0 );
	if ( !pOptions )
	{	oexERROR( 0, oexT( "sendOptionsCmd() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Ditch the options
	delete [] pOptions;
	pOptions = oexNULL;

	char *pSdp = oexNULL;
	if ( m && m->isset( oexT( "username" ) ) )
		pSdp = m_pRtspClient->describeWithPassword( oexStrToMbPtr( sUrl.c_str() ), 
													oexStrToMbPtr( (*m)[ oexT( "username" ) ].str().c_str() ), 
													oexStrToMbPtr( (*m)[ oexT( "password" ) ].str().c_str() ) );
	else
		pSdp = m_pRtspClient->describeURL( oexStrToMbPtr( sUrl.c_str() ) );
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
	int bFoundVideo = 0, bFoundAudio = 0;
	oex::CStr8 sVTag = "video", sATag = "audio";
	while ( 0 != ( pss = iter.next() ) 
			&& ( ( bVideo && !bFoundVideo ) || ( bAudio && !bFoundAudio ) ) )
	{
//		oexEcho( oexMks( pss->mediumName(), " - ", pss->codecName() ).Ptr() )

		if ( bVideo && !bFoundVideo && sVTag == pss->mediumName() )
			InitVideo( pss ), bFoundVideo = 1;

		else if ( bAudio && !bFoundAudio && sATag == pss->mediumName() )
			InitAudio( pss ), bFoundAudio = 1;

	} // end while

	m_bVideo = bFoundVideo;
	m_bAudio = bFoundAudio;

//	m_pRtspClient->playMediaSession( *m_pSession, 0, 0, 1.f );
	m_pRtspClient->playMediaSession( *m_pSession, 0, -1.f, 1.f );

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

int CLvRtspClient::InitVideo( MediaSubsession *pss )
{_STT();
	if ( !pss )
	{	oexERROR( 0, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate( -1 ) )
	{	oexERROR( 0, oexT( "initiate() video stream failed" ) );
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	oexERROR( 0, oexT( "RTP source is null" ) );
		return 0;
	} // end if

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	int sn = pss->rtpSource()->RTPgs()->socketNum();
	setReceiveBufferTo( *m_pEnv, sn, 1000000 );

	if ( pss->codecName() )
		m_sVideoCodec = oexMbToStrPtr( pss->codecName() );

	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
	{	oexERROR( 0, oexMks( oexT( "setupMediaSubsession() failed, Codec : " ), m_sVideoCodec.c_str() ).Ptr()  );
		return 0;
	} // end if

	m_pVs = new CVideoSink( *m_pEnv );
	if ( !m_pVs )
	{	oexERROR( 0, oexT( "CVideoSink::createNew() failed" ) );
		return 0;
	} // end if

	if ( !m_pVs->startPlaying( *pss->rtpSource(), 0, 0 ) )
	{	oexERROR( 0, oexT( "CVideoSink::startPlaying() failed" ) );
		return 0;
	} // end if

	return 1;
}

int CLvRtspClient::InitAudio( MediaSubsession *pss )
{_STT();
	if ( !pss )
	{	oexERROR( 0, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate( -1 ) )
	{	oexERROR( 0, oexT( "initiate() video stream failed" ) );
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	oexERROR( 0, oexT( "RTP source is null" ) );
		return 0;
	} // end if

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	int sn = pss->rtpSource()->RTPgs()->socketNum();
	setReceiveBufferTo( *m_pEnv, sn, 1000000 );

	if ( pss->codecName() )
		m_sAudioCodec = oexMbToStrPtr( pss->codecName() );

	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
	{	oexERROR( 0, oexT( "setupMediaSubsession() failed" ) );
		return 0;
	} // end if

	m_pAs = new CAudioSink( *m_pEnv );
	if ( !m_pAs )
	{	oexERROR( 0, oexT( "CAudioSink::createNew() failed" ) );
		return 0;
	} // end if

	if ( !m_pAs->startPlaying( *pss->rtpSource(), 0, 0 ) )
	{	oexERROR( 0, oexT( "CAudioSink::startPlaying() failed" ) );
		return 0;
	} // end if

	return 1;
}

int CLvRtspClient::LockVideo( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	if ( !m_bVideo || !m_pVs )
		return 0;

	return m_pVs->LockFrame( dat, m );
}

int CLvRtspClient::UnlockVideo()
{_STT();
	if ( !m_pVs )
		return 0;

	return m_pVs->UnlockFrame();
}

int CLvRtspClient::LockAudio( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	if ( !m_bAudio || !m_pAs )
		return 0;

	return m_pAs->LockFrame( dat, m );
}

int CLvRtspClient::UnlockAudio()
{_STT();
	if ( !m_pAs )
		return 0;

	return m_pAs->UnlockFrame();
}

oex::oexBOOL CLvRtspClient::InitThread( oex::oexPVOID x_pData )
{_STT();
	if ( !ThreadOpen( m_sUrl, m_bVideo, m_bAudio, &m_mParams ) )
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspClient::DoThread( oex::oexPVOID x_pData )
{_STT();
	// Run the thing
	if ( !m_pEnv )
		return oex::oexFALSE;

	m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	return oex::oexFALSE;
}

oex::oexINT CLvRtspClient::EndThread( oex::oexPVOID x_pData )
{_STT();
	ThreadDestroy();

	return 0;
}


