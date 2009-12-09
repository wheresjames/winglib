// lv_rtsp_server.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"


// CLiveMediaSubsession::createNew
CLvRtspServer::CLiveMediaSubsession* CLvRtspServer::CLiveMediaSubsession::createNew( UsageEnvironment& env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer )
{
oexM();

	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSubsession( env, reuseFirstSource, pRtspServer );
}

// CLiveMediaSubsession::createNewStreamSource
FramedSource* CLvRtspServer::CLiveMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
oexM();

	if ( !m_pRtspServer )
		return oexNULL;

	CLvRtspServer::CLiveMediaSource *pSource
		= CLvRtspServer::CLiveMediaSource::createNew( envir(), m_pRtspServer );

	estBitrate = 500;

	// Create a framer for the video stream
//	return MPEG4VideoStreamFramer::createNew( envir(), pSource );
	return MPEG4VideoStreamDiscreteFramer::createNew( envir(), pSource );
}

// CLiveMediaSubsession::createNewRTPSink
RTPSink* CLvRtspServer::CLiveMediaSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
oexM();
	return MPEG4ESVideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000 );

}

// CLiveMediaSource::createNew()
CLvRtspServer::CLiveMediaSource* CLvRtspServer::CLiveMediaSource::createNew( UsageEnvironment& env, CLvRtspServer *pRtspServer )
{
oexM();

	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSource( env, pRtspServer );
}

// CLiveMediaSource::doGetNextFrame()
void CLvRtspServer::CLiveMediaSource::doGetNextFrame()
{
oexM();

	// Call into the script to get the next frame
	if ( !m_pRtspServer || 0 > m_pRtspServer->CallDoGetNextFrame( &CLvRtspServer::CLiveMediaSource::_deliverFrame, this ) )
	{	handleClosure(this);
		return;
	} // end if
}

void CLvRtspServer::CLiveMediaSource::_deliverFrame( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame )
{
	CLvRtspServer::CLiveMediaSource *p = (CLvRtspServer::CLiveMediaSource*)pUserData;
	if ( p )
		p->deliverFrame( pFrame );
}

// CLiveMediaSource::deliverFrame()
void CLvRtspServer::CLiveMediaSource::deliverFrame( sqbind::CSqBinary *pFrame )
{
oexM();

	// Are we waiting for data?
//	if ( !isCurrentlyAwaitingData() )
//		return;

	if ( !pFrame )
	{	fFrameSize = 0;
		fTo = 0;
	} // end if

	else
	{
		fFrameSize = pFrame->getUsed();
		if ( fFrameSize > fMaxSize )
			fFrameSize = fMaxSize;

		fTo = (unsigned char*)pFrame->_Ptr();

	} // end else

	// Deliver the data here:

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting( this );
}

CLvRtspServer::CLvRtspServer()
{
	m_nEnd = 0;
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
	m_pMsgQueue = oexNULL;
	m_fDeliverFrame = oexNULL;
	m_pUserData = oexNULL;
}

void CLvRtspServer::Destroy()
{
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
{
	Destroy();

	if ( m )
		m_mParams = *m;

	Start();

	return 1;
}

oex::oexBOOL CLvRtspServer::InitThread( oex::oexPVOID x_pData )
{
	// Initialize the video server
	if ( !ThreadOpen( &m_mParams ) )
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspServer::DoThread( oex::oexPVOID x_pData )
{
	// Run the thing
	if ( !m_pEnv )
		return oex::oexFALSE;

	// Do the stuff
	m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	oexEcho( "doEventLoop() exited" );

	return oex::oexFALSE;
}

oex::oexINT CLvRtspServer::EndThread( oex::oexPVOID x_pData )
{
	// Close everything
	ThreadDestroy();

	return 0;
}

void CLvRtspServer::ThreadDestroy()
{
	if ( m_pEnv )
		m_pEnv->reclaim();

//	if ( m_pRtspServer )
//		m_pRtspServer->close();

	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
}

int CLvRtspServer::ThreadOpen( sqbind::CSqMulti *m )
{
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
	{	oexERROR( 0, oexMks( oexT( "RTSPServer::createNew() failed : " ), m_pEnv->getResultMsg() ) );
		ThreadDestroy();
		return 0;
	} // end if

	const char *pStreamName = "vid1";
	const char *pStreamDesc = "Testing the Live555 API";

	// Create media session
    ServerMediaSession* pSms = ServerMediaSession::createNew( *m_pEnv, pStreamName, pStreamName, pStreamDesc );
	if ( !pSms )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::createNew() failed : " ), m_pEnv->getResultMsg() ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Add subsession
    if ( !pSms->addSubsession( CLiveMediaSubsession::createNew( *m_pEnv, True, this ) ) )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::addSubsession() failed : " ), m_pEnv->getResultMsg() ) );
		ThreadDestroy();
		return 0;
	} // end if

	/// Add session option to server
	m_pRtspServer->addServerMediaSession( pSms );

	AnnounceStream( m_pRtspServer, pSms, pStreamName );

	return 1;
}

void CLvRtspServer::AnnounceStream( RTSPServer* pRtspServer, ServerMediaSession* pSms, char const* pStreamName )
{
	char* url = pRtspServer->rtspURL( pSms );

	pRtspServer->envir() 	<< "\n\"" << pStreamName << "\" stream\n"
							<< "Play this stream using the URL \"" << url << "\"\n";

	delete[] url;
}

int CLvRtspServer::CallDoGetNextFrame( f_deliverFrame fDeliverFrame, oex::oexPVOID pUserData )
{
	m_fDeliverFrame = fDeliverFrame;
	m_pUserData = pUserData;

	if ( !m_pMsgQueue || !m_sDoGetNextFrame.length() )
		return -1;
	sqbind::stdString sReply;
	if ( !m_pMsgQueue->execute( &sReply, oexT( "." ), m_sDoGetNextFrame.c_str() ) )
		return -2;

	return oexStrToLong( sReply.c_str() );
}

