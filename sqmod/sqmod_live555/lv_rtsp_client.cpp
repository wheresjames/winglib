// lv_rtsp_client.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>


CLvRtspClient::CVideoSink::CVideoSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{_STT();

	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
}

CLvRtspClient::CVideoSink::~CVideoSink()
{_STT();
	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
	m_buf.Free();
}

void CLvRtspClient::CVideoSink::setHeader( sqbind::CSqBinary *header )
{_STT();
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return;
	if ( header )
		m_header = *header;
}

Boolean CLvRtspClient::CVideoSink::continuePlaying()
{_STT();
	if ( !fSource )
		return False;

	m_nFrameGrabbing = 1;

	if ( !m_buf.Size() )
	{
		m_buf.Mem().Mem().OexNew( eDefaultBufferSize );

		oexAutoLock ll( m_lock );
		if ( ll.IsLocked() )
		{
			m_buf.MemCpyAt( &m_header, 0 );

#if defined( oexDEBUG )
//			oexSHOW( m_header.getUsed() );
//			oexEcho( oexBinToAsciiHexStr( m_header.Mem(), 0, 16, 16 ).Ptr() );
#endif
		} // end if

	} // end if

	fSource->getNextFrame( (unsigned char*)m_buf._Ptr( m_header.getUsed() ), m_buf.Size(), _afterGettingFrame, this, onSourceClosure, this );

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
	m_buf.setUsed( m_header.getUsed() + frameSize );

	// Signal that a frame is ready
	m_nFrameReady = 1;
	m_nFrameGrabbing = 0;

}

int CLvRtspClient::CVideoSink::needFrame()
{
	if ( m_nFrameGrabbing )
		return 0;

	if ( m_nFrameReady )
		return 0;

	return 1;
}

int CLvRtspClient::CVideoSink::LockFrame( sqbind::CSqBinary *dat, int to )
{_STT();

	// Check to see if a new frame is ready
	if ( !m_nFrameReady )
		return 0;

	if ( dat )
		*dat = m_buf;

	return 1;
}

int CLvRtspClient::CVideoSink::UnlockFrame()
{_STT();

	m_nFrameReady = 0;

	return 1;
}


CLvRtspClient::CAudioSink::CAudioSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{_STT();
	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
}

CLvRtspClient::CAudioSink::~CAudioSink()
{_STT();
	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
	m_buf.Free();
}

Boolean CLvRtspClient::CAudioSink::continuePlaying()
{_STT();
	if ( !fSource )
		return False;

	m_nFrameGrabbing = 1;

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

	// Signal that a frame is ready
	m_nFrameReady = 1;
	m_nFrameGrabbing = 0;
}

int CLvRtspClient::CAudioSink::needFrame()
{
	if ( m_nFrameGrabbing )
		return 0;

	if ( m_nFrameReady )
		return 0;

	return 1;
}

int CLvRtspClient::CAudioSink::LockFrame( sqbind::CSqBinary *dat, int to )
{_STT();

	// Check to see if a new frame is ready
	if ( !m_nFrameReady )
		return 0;

	if ( dat )
		*dat = m_buf;

	return 1;
}

int CLvRtspClient::CAudioSink::UnlockFrame()
{_STT();

	// Unlock the buffer
	m_nFrameReady = 0;

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
	m_width = 0;
	m_height = 0;
	m_fps = 0;
	m_pVsPss = oexNULL;
	m_pAsPss = oexNULL;
}

void CLvRtspClient::Destroy()
{_STT();

	// Tell thread loop to exit
	m_nEnd = 1;

	Stop();

	// Verify things shutdown smoothly
	if ( m_pRtspClient || m_pSession || m_pVs || m_pAs || m_pEnv )
	{	oexERROR( 0, oexT( "RTSP Client failed to shutdown correctly" ) );
	} // end if			

	m_sUrl = oexT( "" );
	m_mParams.clear();
}

void CLvRtspClient::ThreadDestroy()
{_STT();

	if ( m_pRtspClient && m_pSession )
		m_pRtspClient->teardownMediaSession( *m_pSession );

	if ( m_pVs )
		delete m_pVs;

	if ( m_pAs )
		delete m_pAs;

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
	m_width = 0;
	m_height = 0;
	m_fps = 0;
	m_pVsPss = oexNULL;
	m_pAsPss = oexNULL;
	m_mSdp.clear();
}

int CLvRtspClient::Open( const sqbind::stdString &sUrl, int bVideo, int bAudio, sqbind::CSqMulti *m )
{_STT();
	Destroy();

	m_sUrl = sUrl;
	if ( m )
		m_mParams = *m;

	m_bVideo = bVideo;
	m_bAudio = bAudio;

	m_nEnd = 0;
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
	{	oexERROR( 0, oexMks( oexT( "BasicTaskScheduler::createNew() failed : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	m_pEnv = BasicUsageEnvironment::createNew( *scheduler );
	if ( !m_pEnv )
	{	oexERROR( 0, oexMks( oexT( "BasicUsageEnvironment::createNew() failed : " ), m_pEnv->getResultMsg() ) );
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
	{	oexERROR( 0, oexMks( oexT( "RTSPClient::createNew() failed : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	char *pOptions = m_pRtspClient->sendOptionsCmd( oexStrToMbPtr( sUrl.c_str() ), 0, 0 );
	if ( !pOptions )
	{//	oexERROR( 0, oexMks( oexT( "sendOptionsCmd() failed : " ), m_pEnv->getResultMsg() ) );
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
	{	oexERROR( 0, oexMks( oexT( "describeURL() failed : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	// Parse params
	m_mSdp.parse( oexMbToStrPtr( pSdp ), oexT( "\r\n" ), oexT( ":" ), 1 );

	// Create session
	m_pSession = MediaSession::createNew( *m_pEnv, pSdp );

	// Ditch the SDP description
	delete [] pSdp;
	pSdp = oexNULL;

	if ( !m_pSession )
	{	oexERROR( 0, oexMks( oexT( "MediaSession::createNew() failed : " ), m_pEnv->getResultMsg() ) );
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
		if ( bVideo && !bFoundVideo && sVTag == pss->mediumName() )
			InitVideo( pss ), bFoundVideo = 1;

		else if ( bAudio && !bFoundAudio && sATag == pss->mediumName() )
			InitAudio( pss ), bFoundAudio = 1;

	} // end while

	m_bVideo = bFoundVideo;
	m_bAudio = bFoundAudio;

//	m_pRtspClient->playMediaSession( *m_pSession, 0, 0, 1.f );
//	m_pRtspClient->playMediaSession( *m_pSession, 0, -1.f, 1.f );

	return 1;
}

int CLvRtspClient::InitVideo( MediaSubsession *pss )
{_STT();

	_STT_SET_CHECKPOINT( 1 );

	if ( !m_pRtspClient )
	{	oexERROR( 0, oexT( "Invalid rtsp client object" ) );
		return 0;
	} // end if

	if ( !pss )
	{	oexERROR( 0, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	_STT_SET_CHECKPOINT( 2 );

	// Create receiver for stream
	if ( !pss->initiate( -1 ) )
	{	oexERROR( 0, oexMks( oexT( "initiate() video stream failed : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	_STT_SET_CHECKPOINT( 3 );

	if ( !pss->rtpSource() )
	{	oexERROR( 0, oexMks( oexT( "RTP source is null : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	// +++ Think fmtp_config() needs to be handled

	_STT_SET_CHECKPOINT( 4 );

	// sprop-parameter-sets= base64(SPS),base64(PPS)[,base64(SEI)]
	const char *props = pss->fmtp_spropparametersets();
	if ( props )
	{	oex::TList< oex::CStr8 > lst = oex::CParser::Explode( props, oexT( "," ) );
		for ( oex::TList< oex::CStr8 >::iterator it; lst.Next( it ); )
		{	m_extraVideo.AppendBuffer( "\x00\x00\x01", 3 );
			m_extraVideo.Mem().appendString( oex::CBase64::Decode( *it ) );
		} // end for
	} // end if

	_STT_SET_CHECKPOINT( 5 );

	// Try to get the image width / height
	m_width = pss->videoWidth();
	m_height = pss->videoWidth();
	m_fps = pss->videoFPS();

	_STT_SET_CHECKPOINT( 6 );

	// a=framesize:96 176-144
	if ( !m_width || !m_height )
	{	sqbind::stdString s = m_mSdp[ oexT( "a=framesize" ) ].str();
		if ( s.length() )
		{	oex::TList< oex::CStr > lst = oex::CParser::Split( s.c_str(), s.length(), oexT( " " ) );
			if ( 1 < lst.Size() )
			{	oex::TList< oex::CStr > nums = oex::CParser::Split( lst[ 1 ]->Ptr(), lst[ 1 ]->Length(), oexT( "-" ) );
				if ( 1 < nums.Size() )
					m_width = nums[ 0 ]->ToLong(),
					m_height = nums[ 1 ]->ToLong();
			} // end if
		} // end if
	} // end if

	_STT_SET_CHECKPOINT( 7 );

	// a=cliprect:0,0,144,176
	if ( !m_width || !m_height )
	{	sqbind::stdString s = m_mSdp[ oexT( "a=cliprect" ) ].str();
		if ( s.length() )
		{	oex::TList< oex::CStr > nums = oex::CParser::Split( s.c_str(), s.length(), oexT( "," ) );
			if ( 3 < nums.Size() )
				m_width = nums[ 3 ]->ToLong(),
				m_height = nums[ 2 ]->ToLong();
		} // end if
	} // end if

	_STT_SET_CHECKPOINT( 8 );

	// a=framerate:30
	if ( !m_fps )
		m_fps = m_mSdp[ oexT( "a=framerate" ) ].toint();

	// a=maxprate:30
//	if ( !m_fps )
	//	m_fps = m_mSdp[ oexT( "a=maxprate" ) ].toint();

	_STT_SET_CHECKPOINT( 9 );

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	_STT_SET_CHECKPOINT( 10 );

	int sn = pss->rtpSource()->RTPgs()->socketNum();
	setReceiveBufferTo( *m_pEnv, sn, 2000000 );

	_STT_SET_CHECKPOINT( 11 );

	if ( pss->codecName() )
		m_sVideoCodec = oexMbToStrPtr( pss->codecName() );

	_STT_SET_CHECKPOINT( 12 );

	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
	{	oexERROR( 0, oexMks( oexT( "setupMediaSubsession() failed, Codec : " ), m_sVideoCodec.c_str(), oexT( " : " ), m_pEnv->getResultMsg() ).Ptr() );
		return 0;
	} // end if

	_STT_SET_CHECKPOINT( 13 );

	m_pVs = new CVideoSink( *m_pEnv );
	if ( !m_pVs )
	{	oexERROR( 0, oexMks( oexT( "CVideoSink::createNew() failed" ), oexT( " : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	_STT_SET_CHECKPOINT( 14 );

	m_pVsPss = pss;

	return 1;
}

int CLvRtspClient::InitAudio( MediaSubsession *pss )
{_STT();
	
	if ( !m_pRtspClient )
	{	oexERROR( 0, oexT( "Invalid rtsp client object" ) );
		return 0;
	} // end if

	if ( !pss )
	{	oexERROR( 0, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate( -1 ) )
	{	oexERROR( 0, oexMks( oexT( "initiate() video stream failed : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	oexERROR( 0, oexMks( oexT( "RTP source is null : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	// Read extradata
    const char *props = pss->fmtp_spropparametersets();
    if ( props )
    {	oex::TList< oex::CStr8 > lst = oex::CParser::Explode( props, oexT( "," ) );
        for ( oex::TList< oex::CStr8 >::iterator it; lst.Next( it ); )
        {	m_extraAudio.AppendBuffer( "\x00\x00\x01", 3 );
            m_extraAudio.Mem().appendString( oex::CBase64::Decode( *it ) );
        } // end for
    } // end if

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	int sn = pss->rtpSource()->RTPgs()->socketNum();
	setReceiveBufferTo( *m_pEnv, sn, 2000000 );

	if ( pss->codecName() )
		m_sAudioCodec = oexMbToStrPtr( pss->codecName() );

	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
	{	oexERROR( 0, oexMks( oexT( "setupMediaSubsession() failed : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	m_pAs = new CAudioSink( *m_pEnv );
	if ( !m_pAs )
	{	oexERROR( 0, oexMks( oexT( "CAudioSink::createNew() failed : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
		return 0;
	} // end if

	m_pAsPss = pss;

	return 1;
}

int CLvRtspClient::LockVideo( sqbind::CSqBinary *dat, int to )
{_STT();
	if ( !m_bVideo || !m_pVs )
		return 0;

	return m_pVs->LockFrame( dat, to );
}

int CLvRtspClient::UnlockVideo()
{_STT();
	if ( !m_pVs )
		return 0;

	return m_pVs->UnlockFrame();
}

int CLvRtspClient::LockAudio( sqbind::CSqBinary *dat, int to )
{_STT();
	if ( !m_bAudio || !m_pAs )
		return 0;

	return m_pAs->LockFrame( dat, to );
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
	{	ThreadDestroy();
		return oex::oexFALSE;
	} // end if

	if ( !m_pRtspClient )
	{	ThreadDestroy();
		return oex::oexFALSE;
	} // end if

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspClient::DoThread( oex::oexPVOID x_pData )
{_STT();

	// Environment?
	if ( !m_pEnv )
		return oex::oexFALSE;

	// Wait for play command
	if ( 0 > m_evtPlay.Wait( GetStopEvent(), m_evtPlay.cInfinite() ) )
		return oex::oexFALSE;

	if ( m_pVs && m_pVsPss )
		if ( !m_pVs->startPlaying( *m_pVsPss->rtpSource(), 0, 0 ) )
		{	oexERROR( 0, oexMks( oexT( "CVideoSink::startPlaying() failed : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
			return 0;
		} // end if

	if ( m_pAs && m_pAsPss )
		if ( !m_pAs->startPlaying( *m_pAsPss->rtpSource(), 0, 0 ) )
		{	oexERROR( 0, oexMks( oexT( "CAudioSink::startPlaying() failed : " ), oexT( " : " ), m_pEnv->getResultMsg() ) );
			return 0;
		} // end if

	// Let's go...
	m_pRtspClient->playMediaSession( *m_pSession, 0, -1.f, 1.f );

	// Schedule idle processing
	m_pEnv->taskScheduler().scheduleDelayedTask( 15, (TaskFunc*)CLvRtspClient::_OnIdle, this );

	// Run the event loop
	m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	return oex::oexFALSE;
}

oex::oexINT CLvRtspClient::EndThread( oex::oexPVOID x_pData )
{_STT();
	ThreadDestroy();

	return 0;
}

void CLvRtspClient::_OnIdle( void *pData )
{_STT();
	CLvRtspClient *p = (CLvRtspClient*)pData;
	if ( p ) p->OnIdle();
}

void CLvRtspClient::OnIdle()
{_STT();

	if ( !m_pEnv )
		return;

	// Request new video frame if needed
	if ( m_pVs && m_pVs->needFrame() )
		m_pVs->continuePlaying();

	// Request new audio frame if needed
	if ( m_pAs && m_pAs->needFrame() )
		m_pAs->continuePlaying();

	// Schedule us to run again
	if ( !m_nEnd )
		m_pEnv->taskScheduler().scheduleDelayedTask( 15, (TaskFunc*)CLvRtspClient::_OnIdle, this );
}

