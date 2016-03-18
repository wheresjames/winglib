// lv_rtsp_client.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>


// CLvRtspClient
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspClient, CLvRtspClient )

	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Open )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Play )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, waitInit )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isOpen )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getWidth )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getHeight )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFps )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setWidth )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setHeight )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setFps )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFrameCount )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getExtraVideoData )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getExtraAudioData )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setVideoHeader )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getSDP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getSDPValue )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setStreamOverTCP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getStreamOverTCP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setTunnelOverHTTPPort )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getTunnelOverHTTPPort )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getNumAudioChannels )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioSampleRate )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioBps )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPtsSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPtsUSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoDts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPtsSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPtsUSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioDts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setBlindLogin )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getBlindLogin )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getError )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getLastError )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getUrl )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getRxBufferSize )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setRxBufferSize )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, EnableDebugInfo )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )

SQBIND_REGISTER_CLASS_END()

oexLock 					CLvRtspClient::m_lockCallbackPtrMap;
CLvRtspClient::t_PtrMap 	CLvRtspClient::m_mCallbackPtrMap;

void CLvRtspClient::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CLvRtspClient );
}

CLvRtspClient::CVideoSink::CVideoSink( UsageEnvironment& rEnv ) :
	MediaSink( rEnv )
{_STT();

	m_pevtData = 0;
	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
}

CLvRtspClient::CVideoSink::~CVideoSink()
{_STT();
	m_pevtData = 0;
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

	fSource->getNextFrame( (unsigned char*)m_buf._Ptr( m_header.getUsed() ), m_buf.Size(),
						   _afterGettingFrame, this, onSourceClosure, this );

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

#if defined( oexDEBUG )
//	oexSHOW( frameSize );
//	oexEcho( oexBinToAsciiHexStr( m_buf.Mem(), 0, 16, 16 ).Ptr() );
//	oexFlush_stdout();
#endif

	// Copy time stamps
	oex::oexCopyTime( m_ts, presentationTime );
	oex::oexSetTime( m_ds, 0, durationInMicroseconds );

	// Signal that a frame is ready
	m_nFrameReady = 1;
	m_nFrameGrabbing = 0;
	if ( m_pevtData )
		m_pevtData->Signal();

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
	// MPEG4LATMAudioRTPSink( rEnv )
{_STT();
	m_pevtData = 0;
	m_nFrameReady = 0;
	m_nFrameGrabbing = 0;
}

CLvRtspClient::CAudioSink::~CAudioSink()
{_STT();
	m_pevtData = 0;
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

	fSource->getNextFrame( (unsigned char*)m_buf._Ptr(), m_buf.Size(),
						   _afterGettingFrame, this, onSourceClosure, this );

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

	// Copy time stamps
	oex::oexCopyTime( m_ts, presentationTime );
	oex::oexSetTime( m_ds, 0, durationInMicroseconds );

	// Signal that a frame is ready
	m_nFrameReady = 1;
	m_nFrameGrabbing = 0;
	if ( m_pevtData )
		m_pevtData->Signal();
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
	m_nDebug = 0;
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
	m_bStreamOverTCP = 0;
	m_nTunnelOverHTTPPort = 0;
	m_nAudioNumChannels = 0;
	m_nAudioRate = 0;
	m_nAudioBps = 0;
	m_bBlindLogin = 1;
	m_nLastError = 0;
	m_nRxBufferSize = 2000000;
	m_bAuthenticate = 0;
}

void CLvRtspClient::Destroy()
{_STT();

	// Tell thread loop to exit
	m_nEnd = 1;

	// Stop the thread
	Stop();

	// Verify things shutdown smoothly
	if ( m_pRtspClient || m_pSession || m_pVs || m_pAs || m_pEnv )
	{	setLastError( -999, oexT( "RTSP Client failed to shutdown correctly" ) );
	} // end if

	m_sUrl = oexT( "" );
	m_mParams.clear();

	m_nLastError = 0;
	m_sLastError = oexT( "" );

	m_bAuthenticate = 0;
	m_authenticator.reset();
}

int CLvRtspClient::setLastError( int e, sqbind::stdString s )
{
	if ( m_nLastError || !e )
		return m_nLastError;

	m_sLastError = s;
	m_nLastError = e;

	return e;
}

void CLvRtspClient::ThreadDestroy()
{_STT();

	if ( m_pRtspClient && m_pSession )
	{
		m_pevtCallbackDone.Reset();
		m_pRtspClient->sendTeardownCommand( *m_pSession, _OnResponseHandler );

		if ( m_pevtCallbackDone.Wait( GetStopEvent(), 0 ) )
			if ( m_pEnv )
				setLastError( -1, sqbind::oex2std( oexMks( oexT( "teardownMediaSession() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );

//		m_pSession = oexNULL;

	} // end if

	if ( m_pVs )
		delete m_pVs, m_pVs = oexNULL;

	if ( m_pAs )
		delete m_pAs, m_pAs = oexNULL;

	if ( m_pRtspClient )
	{
		oexAutoLock ll( m_lockCallbackPtrMap );
		if ( ll.IsLocked() )
		{	t_PtrMap::iterator it = m_mCallbackPtrMap.find( m_pRtspClient );
			if ( m_mCallbackPtrMap.end() != it )
				m_mCallbackPtrMap.erase( it );
		} // end if

		if ( m_pSession )
			m_pRtspClient->close( m_pSession );

		Medium::close( m_pRtspClient );

	} // end if

	if ( m_pEnv )
	{
		m_pEnv->reclaim();

//		Medium::close( m_pEnv );

	} // end if

	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspClient = oexNULL;
	m_pSession = oexNULL;
	m_sVideoCodec = oexT( "" );
	m_width = 0;
	m_height = 0;
	m_fps = 0;
	m_nAudioNumChannels = 0;
	m_nAudioRate = 0;
	m_nAudioBps = 0;
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

void CLvRtspClient::_OnResponseHandler( RTSPClient *rtspClient, int resultCode, char *resultString )
{
	// Look up object pointer
	CLvRtspClient *pRc = 0;
	{ // Scope
		oexAutoLock ll( m_lockCallbackPtrMap );
		if ( ll.IsLocked() )
		{	t_PtrMap::iterator it = m_mCallbackPtrMap.find( rtspClient );
			if ( m_mCallbackPtrMap.end()!= it && it->second )
				pRc = it->second;
		} // end if
	} // end scope

	if ( !pRc )
		return;

	pRc->OnResponseHandler( resultCode, resultString );
}

void CLvRtspClient::OnResponseHandler( int resultCode, char *resultString )
{
	// Save result
	m_nCallbackResult = resultCode;

	if ( resultString )
//		printf( "%s(%li) %s\n", __FILE__, __LINE__, resultString ), fflush(stdout),
		m_sCallbackResult = resultString;
	else
		m_sCallbackResult.clear();

	// Callback complete
	m_pevtCallbackDone.Signal();

	m_nLoop = 1;

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
	{	setLastError( -1, sqbind::oex2std( oexMks( oexT( "BasicTaskScheduler::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_pEnv = BasicUsageEnvironment::createNew( *scheduler );
	if ( !m_pEnv )
	{	setLastError( -2, sqbind::oex2std( oexMks( oexT( "BasicUsageEnvironment::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	// Create rtsp client
//	m_pRtspClient = RTSPClient::createNew( *m_pEnv, m_nDebug, "CLvRtspClient", m_nTunnelOverHTTPPort );
	m_pRtspClient = RTSPClient::createNew( *m_pEnv, oexStrToMb( sqbind::std2oex( sUrl ) ).c_str(), m_nDebug, "CLvRtspClient", m_nTunnelOverHTTPPort );
	if ( !m_pRtspClient )
	{	setLastError( -3, sqbind::oex2std( oexMks( oexT( "RTSPClient::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg()) ) ) );
		return 0;
	} // end if

	// Set class pointer for callback
	{ // Scope
		oexAutoLock ll( m_lockCallbackPtrMap );
		if ( ll.IsLocked() )
			m_mCallbackPtrMap[ m_pRtspClient ] = this;
	} // end scope

	// Do we need to authenticate?
	if ( m_bBlindLogin && m && m->isset( oexT( "username" ) ) )
		m_bAuthenticate = 1,
		m_authenticator.setUsernameAndPassword( (char*)oexStrToMbPtr( (*m)[ oexT( "username" ) ].str().c_str() ),
												(char*)oexStrToMbPtr( (*m)[ oexT( "password" ) ].str().c_str() ) );


	unsigned uRes = 0;

	// Send options command
	m_pevtCallbackDone.Reset();
	uRes = m_pRtspClient->sendOptionsCommand( _OnResponseHandler, m_bAuthenticate ? &m_authenticator : 0 );
	if ( !uRes )
	{	setLastError( -4, sqbind::oex2std( oexMks( oexT( "sendOptionsCommand() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_nLoop = 0;
	m_pEnv->taskScheduler().doEventLoop( &m_nLoop );

	if ( m_pevtCallbackDone.Wait( GetStopEvent(), 0 ) )
	{	setLastError( -5, sqbind::oex2std( oexMks( oexT( "sendOptionsCommand() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( m_nCallbackResult )
	{	setLastError( -6, sqbind::oex2std( oexMks( oexT( "sendOptionsCommand() Callback Failed : " ), m_sCallbackResult.c_str(), oexT( " : " ), m_nCallbackResult ) ) );
		return 0;
	} // end if

/*
	char *pOptions = oexNULL;
	if ( m_bBlindLogin && m && m->isset( oexT( "username" ) ) )
		pOptions = m_pRtspClient->sendOptionsCmd( oexStrToMbPtr( sUrl.c_str() ),
												  (char*)oexStrToMbPtr( (*m)[ oexT( "username" ) ].str().c_str() ),
												  (char*)oexStrToMbPtr( (*m)[ oexT( "password" ) ].str().c_str() ) );
	else
		pOptions = m_pRtspClient->sendOptionsCmd( oexStrToMbPtr( sUrl.c_str() ), 0, 0 );

	if ( !pOptions )
	{	setLastError( -4, sqbind::oex2std( oexMks( oexT( "sendOptionsCmd() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	// Ditch the options
	delete pOptions;
	pOptions = oexNULL;
*/

/*
	char *pSdp = oexNULL;
	if ( m && m->isset( oexT( "username" ) ) )
		pSdp = m_pRtspClient->describeWithPassword( oexStrToMbPtr( sUrl.c_str() ),
													oexStrToMbPtr( (*m)[ oexT( "username" ) ].str().c_str() ),
													oexStrToMbPtr( (*m)[ oexT( "password" ) ].str().c_str() ) );
	else
		pSdp = m_pRtspClient->describeURL( oexStrToMbPtr( sUrl.c_str() ) );

	if ( !pSdp )
	{	setLastError( -5, sqbind::oex2std( oexMks( oexT( "describeURL() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if
*/

	// Send options command
	m_pevtCallbackDone.Reset();
	uRes = m_pRtspClient->sendDescribeCommand( _OnResponseHandler, m_bAuthenticate ? &m_authenticator : 0 );
	if ( !uRes )
	{	setLastError( 7, sqbind::oex2std( oexMks( oexT( "sendDescribeCommand() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_nLoop = 0;
	m_pEnv->taskScheduler().doEventLoop( &m_nLoop );

	// Wait for callback
	if ( 0 > m_pevtCallbackDone.Wait( GetStopEvent(), 0 ) )
	{	setLastError( -8, sqbind::oex2std( oexMks( oexT( "sendDescribeCommand() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( m_nCallbackResult )
	{	setLastError( -9, sqbind::oex2std( oexMks( oexT( "sendDescribeCommand() Callback Failed : " ), m_nCallbackResult ) ) );
		return 0;
	} // end if

	const char *pSdp = m_sCallbackResult.Ptr();
	if ( !pSdp )
	{	setLastError( -10, sqbind::oex2std( oexMks( oexT( "sendDescribeCommand() returned null SDP" ) ) ) );
		return 0;
	} // end if

	// Parse params
	m_mSdp.parse( oexMbToStrPtr( pSdp ), oexT( "\r\n" ), oexT( ":" ), 1 );

	// Create session
	m_pSession = MediaSession::createNew( *m_pEnv, pSdp );

	// Ditch the SDP description
//	delete pSdp;
	pSdp = oexNULL;

	if ( !m_pSession )
	{	setLastError( -6, sqbind::oex2std( oexMks( oexT( "MediaSession::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
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

	if ( !m_bVideo && !m_bAudio )
	{	setLastError( -7, sqbind::oex2std( oexMks( oexT( "No Video or audio : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

//	m_pRtspClient->playMediaSession( *m_pSession, 0, 0, 1.f );
//	m_pRtspClient->playMediaSession( *m_pSession, 0, -1.f, 1.f );

	return 1;
}

int CLvRtspClient::InitVideo( MediaSubsession *pss )
{_STT();

	if ( !m_pRtspClient )
	{	setLastError( -100, oexT( "Invalid rtsp client object" ) );
		return 0;
	} // end if

	if ( !pss )
	{	setLastError( -101, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate() )
	{	setLastError( -102, sqbind::oex2std( oexMks( oexT( "initiate() video stream failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	setLastError( -103, sqbind::oex2std( oexMks( oexT( "RTP source is null : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	// +++ Think fmtp_config() needs to be handled

	// sprop-parameter-sets= base64(SPS),base64(PPS)[,base64(SEI)]
	const char *props = pss->fmtp_spropparametersets();
	if ( props )
	{	oex::TList< oex::CStr8 > lst = oex::CParser::Explode( props, "," );
		for ( oex::TList< oex::CStr8 >::iterator it; lst.Next( it ); )
		{	m_extraVideo.AppendBuffer( "\x00\x00\x01", 3 );
			m_extraVideo.Mem().appendString( oex::CBase64::Decode( *it ) );
		} // end for
	} // end if
//	oexEcho( oexBinToAsciiHexStr( m_extraVideo.Mem(), 0, 16, 16 ).Ptr() );

	// Try to get the image width / height
	m_width = pss->videoWidth();
	m_height = pss->videoWidth();
	m_fps = pss->videoFPS();

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

	// a=framerate:30
	if ( !m_fps )
		m_fps = m_mSdp[ oexT( "a=framerate" ) ].toint();

	// a=maxprate:30
	if ( !m_fps )
		m_fps = m_mSdp[ oexT( "a=maxprate" ) ].toint();

	// Set minimum rx buffer size
	if ( 2000000 > m_nRxBufferSize )
		m_nRxBufferSize = 2000000;

	// Set rx buffer size
	int sn = pss->rtpSource()->RTPgs()->socketNum();
	increaseReceiveBufferTo( *m_pEnv, sn, m_nRxBufferSize );

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	if ( pss->codecName() )
		m_sVideoCodec = oexMbToStrPtr( pss->codecName() );

//	if ( !m_pRtspClient->setupMediaSubsession( *pss, m_bStreamOverTCP, False ) )
//	{	setLastError( -103, sqbind::oex2std( oexMks( oexT( "setupMediaSubsession() failed, Codec : " ), m_sVideoCodec.c_str(), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
//		return 0;
//	} // end if

	m_pevtCallbackDone.Reset();
	unsigned uRes = m_pRtspClient->sendSetupCommand( *pss, _OnResponseHandler, False, m_bStreamOverTCP, False, m_bAuthenticate ? &m_authenticator : 0 );
	if ( !uRes )
	{	setLastError( -105, sqbind::oex2std( oexMks( oexT( "video: sendSetupCommand() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_nLoop = 0;
	m_pEnv->taskScheduler().doEventLoop( &m_nLoop );

	// Wait for callback
	if ( 0 > m_pevtCallbackDone.Wait( GetStopEvent(), 0 ) )
	{	setLastError( -106, sqbind::oex2std( oexMks( oexT( "video: sendSetupCommand() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( m_nCallbackResult )
	{	setLastError( -107, sqbind::oex2std( oexMks( oexT( "video: sendSetupCommand() Callback Failed : " ), m_nCallbackResult ) ) );
		return 0;
	} // end if

	m_pVs = new CVideoSink( *m_pEnv );
	if ( !m_pVs )
	{	setLastError( -108, sqbind::oex2std( oexMks( oexT( "CVideoSink::createNew() failed" ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_pVs->setDataEvent( &m_evtData );
	m_pVsPss = pss;

	return 1;
}

int CLvRtspClient::InitAudio( MediaSubsession *pss )
{_STT();

	if ( !m_pRtspClient )
	{	setLastError( -200, oexT( "Invalid rtsp client object" ) );
		return 0;
	} // end if

	if ( !pss )
	{	setLastError( -201, oexT( "Invalid video object" ) );
		return 0;
	} // end if

	// Create receiver for stream
	if ( !pss->initiate() )
	{	setLastError( -203, sqbind::oex2std( oexMks( oexT( "initiate() video stream failed : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( !pss->rtpSource() )
	{	setLastError( -204, sqbind::oex2std( oexMks( oexT( "RTP source is null : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( oex::CStr8( "MP4A-LATM" ) == pss->codecName() )
	{
		((MPEG4LATMAudioRTPSource*)pss->rtpSource())->omitLATMDataLengthField();

		const char *pCfg = pss->fmtp_config();
		if ( pCfg )
		{	unsigned elen = 0;
			unsigned char *pExtra = parseStreamMuxConfigStr( pCfg, elen );
			if ( pExtra && elen )
				m_extraAudio.AppendBuffer( (const char*)pExtra, elen );

		} // end if

	} // end if

	else if ( oex::CStr8( "MPEG4-GENERIC" ) == pss->codecName() )
	{
		const char *pCfg = pss->fmtp_config();
		if ( pCfg )
		{	unsigned elen = 0;
			unsigned char *pExtra = parseGeneralConfigStr( pCfg, elen );
			if ( pExtra && elen )
				m_extraAudio.AppendBuffer( (const char*)pExtra, elen );

		} // end if

	} // end if

	// Read extradata
    const char *props = pss->fmtp_spropparametersets();
    if ( props )
    {	oex::TList< oex::CStr8 > lst = oex::CParser::Explode( props, "," );
        for ( oex::TList< oex::CStr8 >::iterator it; lst.Next( it ); )
        {	m_extraAudio.AppendBuffer( "\x00\x00\x01", 3 );
            m_extraAudio.Mem().appendString( oex::CBase64::Decode( *it ) );
        } // end for
    } // end if

	// Set minimum rx buffer size
	if ( 2000000 > m_nRxBufferSize )
		m_nRxBufferSize = 2000000;

	// Set rx buffer size
	int sn = pss->rtpSource()->RTPgs()->socketNum();
	increaseReceiveBufferTo( *m_pEnv, sn, m_nRxBufferSize );

	pss->rtpSource()->setPacketReorderingThresholdTime( 2000000 );

	if ( pss->codecName() )
		m_sAudioCodec = oexMbToStrPtr( pss->codecName() );

//	if ( !m_pRtspClient->setupMediaSubsession( *pss, False, False ) )
//	{	setLastError( -205, sqbind::oex2std( oexMks( oexT( "setupMediaSubsession() failed : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
//		return 0;
//	} // end if

	m_pevtCallbackDone.Reset();
	unsigned uRes = m_pRtspClient->sendSetupCommand( *pss, _OnResponseHandler, False, m_bStreamOverTCP, False, m_bAuthenticate ? &m_authenticator : 0 );
	if ( !uRes )
	{	setLastError( -205, sqbind::oex2std( oexMks( oexT( "audio: sendSetupCommand() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_nLoop = 0;
	m_pEnv->taskScheduler().doEventLoop( &m_nLoop );

	// Wait for callback
	if ( 0 > m_pevtCallbackDone.Wait( GetStopEvent(), 0 ) )
	{	setLastError( -206, sqbind::oex2std( oexMks( oexT( "audio: sendSetupCommand() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( m_nCallbackResult )
	{	setLastError( -207, sqbind::oex2std( oexMks( oexT( "audio: sendSetupCommand() Callback Failed : " ), m_nCallbackResult ) ) );
		return 0;
	} // end if


	// Save away important audio parameters
	m_nAudioNumChannels = pss->numChannels();
	m_nAudioRate = pss->rtpTimestampFrequency();
	m_nAudioBps = 0;

	m_pAs = new CAudioSink( *m_pEnv );
	if ( !m_pAs )
	{	setLastError( -208, sqbind::oex2std( oexMks( oexT( "CAudioSink::createNew() failed : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_pAs->setDataEvent( &m_evtData );
	m_pAsPss = pss;

	return 1;
}

int CLvRtspClient::LockVideo( sqbind::CSqBinary *dat, int to )
{_STT();

	if ( !m_bVideo || !m_pVs )
		return 0;

	// Wait for data?
	if ( to )
		m_evtData.Wait( to );

	// Attempt to lock a frame
	return m_pVs->LockFrame( dat, to );
}

int CLvRtspClient::UnlockVideo()
{_STT();
	if ( !m_pVs )
		return 0;

	// Reset data signal
	m_evtData.Reset();

	return m_pVs->UnlockFrame();
}

int CLvRtspClient::LockAudio( sqbind::CSqBinary *dat, int to )
{_STT();
	if ( !m_bAudio || !m_pAs )
		return 0;

	// Wait for data?
	if ( to )
		m_evtData.Wait( to );

	return m_pAs->LockFrame( dat, to );
}

int CLvRtspClient::UnlockAudio()
{_STT();
	if ( !m_pAs )
		return 0;

	// Reset data signal
	m_evtData.Reset();

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
		{	setLastError( -500, sqbind::oex2std( oexMks( oexT( "CVideoSink::startPlaying() failed : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
			return 0;
		} // end if

	if ( m_pAs && m_pAsPss )
		if ( !m_pAs->startPlaying( *m_pAsPss->rtpSource(), 0, 0 ) )
		{	setLastError( -501, sqbind::oex2std( oexMks( oexT( "CAudioSink::startPlaying() failed : " ), oexT( " : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
			return 0;
		} // end if

	// Let's go...
//	m_pRtspClient->playMediaSession( *m_pSession, 0.f, -1.f, 1.f );

	// Send options command
	m_pevtCallbackDone.Reset();
	unsigned uRes = m_pRtspClient->sendPlayCommand( *m_pSession, _OnResponseHandler, 0.f, -1.f, 1.f, m_bAuthenticate ? &m_authenticator : 0 );
	if ( !uRes )
	{	setLastError( -502, sqbind::oex2std( oexMks( oexT( "sendPlayCommand() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	m_nLoop = 0;
	m_pEnv->taskScheduler().doEventLoop( &m_nLoop );

	// Wait for callback
	if ( 0 > m_pevtCallbackDone.Wait( GetStopEvent(), 8000 ) )
	{	setLastError( -503, sqbind::oex2std( oexMks( oexT( "sendPlayCommand() timed out : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) ) );
		return 0;
	} // end if

	if ( m_nCallbackResult )
	{	setLastError( -504, sqbind::oex2std( oexMks( oexT( "sendPlayCommand() Callback Failed : " ), m_nCallbackResult ) ) );
		return 0;
	} // end if

	// Schedule idle processing
	m_pEnv->taskScheduler().scheduleDelayedTask( 1000, (TaskFunc*)CLvRtspClient::_OnIdle, this );

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
		m_pEnv->taskScheduler().scheduleDelayedTask( 1000, (TaskFunc*)CLvRtspClient::_OnIdle, this );
}

