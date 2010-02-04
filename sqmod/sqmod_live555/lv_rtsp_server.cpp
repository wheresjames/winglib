// lv_rtsp_server.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"


// CLiveMediaSubsession::createNew
CLvRtspServer::CLiveMediaSubsession* CLvRtspServer::CLiveMediaSubsession::createNew( UsageEnvironment& env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer )
{_STT();
	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSubsession( env, reuseFirstSource, pRtspServer );
}

// CLiveMediaSubsession::createNewStreamSource
FramedSource* CLvRtspServer::CLiveMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{_STT();
	if ( !m_pRtspServer )
		return oexNULL;

	CLvRtspServer::CLiveMediaSource *pSource
		= CLvRtspServer::CLiveMediaSource::createNew( envir(), m_pRtspServer );

	estBitrate = 500;

	// Create a framer for the video stream
//	return MPEG4VideoStreamFramer::createNew( envir(), pSource );
	return MPEG4VideoStreamDiscreteFramer::createNew( envir(), pSource );
//	return H264VideoStreamFramer::createNew( envir(), pSource );
//	return MPEG1or2VideoStreamDiscreteFramer::createNew( envir(), pSource );
}

// CLiveMediaSubsession::createNewRTPSink
RTPSink* CLvRtspServer::CLiveMediaSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{_STT();
	return MPEG4ESVideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000000 / 15 );
//	return H264VideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000 );
//	return MPEG1or2VideoRTPSink::createNew( envir(), rtpGroupsock );
}

// CLiveMediaSource::createNew()
CLvRtspServer::CLiveMediaSource* CLvRtspServer::CLiveMediaSource::createNew( UsageEnvironment& env, CLvRtspServer *pRtspServer )
{_STT();
	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSource( env, pRtspServer );
}

// CLiveMediaSource::doGetNextFrame()
void CLvRtspServer::CLiveMediaSource::doGetNextFrame()
{_STT();
	// Call into the script to get the next frame
	if ( !m_pRtspServer || 0 > m_pRtspServer->CallDoGetNextFrame( &CLvRtspServer::CLiveMediaSource::_deliverFrame, this ) )
	{	//handleClosure(this);
		if ( m_pRtspServer )
			m_pRtspServer->Quit();
//		else
//			handleClosure(this);
		return;
	} // end if
}

void CLvRtspServer::CLiveMediaSource::_deliverFrame( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame )
{_STT();
	CLvRtspServer::CLiveMediaSource *p = (CLvRtspServer::CLiveMediaSource*)pUserData;
	if ( p )
		p->deliverFrame( pFrame );
}

// CLiveMediaSource::deliverFrame()
void CLvRtspServer::CLiveMediaSource::deliverFrame( sqbind::CSqBinary *pFrame )
{_STT();
	// End of stream?
	if ( !pFrame || !pFrame->getUsed() )
	{	fFrameSize = 0; fTo = 0;
		if ( m_pRtspServer )
			m_pRtspServer->Quit();
//		else
//			handleClosure(this);
		fPresentationTime.tv_sec == 0;
		fPresentationTime.tv_usec == 0; 
		return;
	} // end if

	// How much data to send?
	fFrameSize = pFrame->getUsed();
	if ( fFrameSize > fMaxSize )
	{	fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	} // end if
	else
		fNumTruncatedBytes = 0;

	// Copy the data
	// +++ anyway to get out of this???
//	fTo = (unsigned char*)pFrame->_Ptr();
	oexMemCpy( fTo, pFrame->Ptr(), fFrameSize );

	double dFps = 15;
	fDurationInMicroseconds = (unsigned int)( double( 1000000 ) / dFps );

	// Use current time on first frame
	if ( fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0 )
		gettimeofday(&fPresentationTime, NULL);

	// Just add the duration
	else
	{	fPresentationTime.tv_usec += fDurationInMicroseconds;
		fPresentationTime.tv_sec += fPresentationTime.tv_usec / 1000000;
		fPresentationTime.tv_usec %= 1000000;
	} // end else

	// Inform the reader that data is now available:

	// This one crashes, I don't quite get the threading issues here...
//	envir().taskScheduler().scheduleDelayedTask( 0, (TaskFunc*)FramedSource::afterGetting, this );

	afterGetting( this );
}

CLvRtspServer::CLvRtspServer()
{_STT();
	m_nEnd = 0;
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
	m_pMsgQueue = oexNULL;
	m_fDeliverFrame = oexNULL;
	m_pUserData = oexNULL;
}

void CLvRtspServer::Destroy()
{_STT();
	// Tell thread loop to exit
	m_nEnd = 1;

	// Stop the thread
	Stop();

	// Lose server params
	m_mParams.clear();

	m_nEnd = 0;
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
}

int CLvRtspServer::StartServer( sqbind::CSqMulti *m )
{_STT();
	Destroy();

	// Save off params
	if ( m )
		m_mParams = *m;

	// Start the thread
	if ( Start() )
		return 0;

	// Wait for the thing to init
	if ( GetInitEvent().Wait( 3000 ) )
		return 0;

	// Did we get a server?
	return oexNULL != m_pRtspServer;
}

oex::oexBOOL CLvRtspServer::InitThread( oex::oexPVOID x_pData )
{_STT();
	// Initialize the video server
	if ( !ThreadOpen( &m_mParams ) )
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspServer::DoThread( oex::oexPVOID x_pData )
{_STT();
	// Run the thing
	if ( !m_pEnv )
		return oex::oexFALSE;

	try
	{
		// Do the stuff
		m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	} // end try
	catch( ... )
	{
	} // end catch

	oexEcho( "doEventLoop() exited" );

	return oex::oexFALSE;
}

oex::oexINT CLvRtspServer::EndThread( oex::oexPVOID x_pData )
{_STT();
	// Close everything
	ThreadDestroy();

	return 0;
}

void CLvRtspServer::ThreadDestroy()
{_STT();
	if ( m_pRtspServer )
		m_pRtspServer->close( m_pRtspServer );

	if ( m_pEnv )
		m_pEnv->reclaim();

	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
}

int CLvRtspServer::ThreadOpen( sqbind::CSqMulti *m )
{_STT();
	// Lose old container
	ThreadDestroy();

	// Setup usage environment
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	if ( !scheduler )
	{	oexERROR( 0, oexT( "BasicTaskScheduler::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Create the environment
	m_pEnv = BasicUsageEnvironment::createNew( *scheduler );
	if ( !m_pEnv )
	{	oexERROR( 0, oexT( "BasicUsageEnvironment::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Authentication module
	UserAuthenticationDatabase* pUad = oexNULL;

	// Create the RTSP server:
	m_pRtspServer = RTSPServer::createNew( *m_pEnv, 8554, pUad );
	if ( !m_pRtspServer )
	{	oexERROR( 0, oexMks( oexT( "RTSPServer::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	const char *pStreamName = "vid1";
	const char *pStreamDesc = "Testing the Live555 API";

	// Create media session
    ServerMediaSession* pSms = ServerMediaSession::createNew( *m_pEnv, pStreamName, pStreamName, pStreamDesc );
	if ( !pSms )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Add subsession
    if ( !pSms->addSubsession( CLiveMediaSubsession::createNew( *m_pEnv, True, this ) ) )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::addSubsession() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	/// Add session option to server
	m_pRtspServer->addServerMediaSession( pSms );

	char* pUrl = m_pRtspServer->rtspURL( pSms );
	if ( pUrl )
	{	m_sUrl = oexMbToStrPtr( pUrl );
		delete [] pUrl;
	} // end if

	return 1;
}

int CLvRtspServer::CallDoGetNextFrame( f_deliverFrame fDeliverFrame, oex::oexPVOID pUserData )
{_STT();
	m_fDeliverFrame = fDeliverFrame;
	m_pUserData = pUserData;

	if ( !m_pMsgQueue || !m_sDoGetNextFrame.length() )
		return -1;
/*
	// +++ Hmmm... I think it should work async, but it stalls
	if ( !m_pMsgQueue->execute( oexNULL, oexT( "." ), m_sDoGetNextFrame.c_str() ) )
		return -2;

	return 0;
*/
	sqbind::stdString sReply;
	if ( !m_pMsgQueue->execute( &sReply, oexT( "." ), m_sDoGetNextFrame.c_str() ) )
		return -2;

	return oexStrToLong( sReply.c_str() );

}

