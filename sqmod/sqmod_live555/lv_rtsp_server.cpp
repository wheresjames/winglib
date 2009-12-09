// lv_rtsp_server.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"


CLvRtspServer::CLiveMediaSubsession* CLvRtspServer::CLiveMediaSubsession::createNew( UsageEnvironment& env, Boolean reuseFirstSource )
{
	return new CLvRtspServer::CLiveMediaSubsession( env, reuseFirstSource );
}

FramedSource* CLvRtspServer::CLiveMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
	estBitrate = 500;

oexEcho( oexMks( " === ", __FUNCTION__ ).Ptr() );

	return CLvRtspServer::CLiveMediaSource::createNew( envir() );
}

RTPSink* CLvRtspServer::CLiveMediaSubsession::createNewRTPSink( Groupsock* rtpGroupsock,
															  	unsigned char rtpPayloadTypeIfDynamic,
															  	FramedSource* inputSource )
{
oexEcho( oexMks( " === ", __FUNCTION__ ).Ptr() );

	return MPEG4ESVideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic );
}

CLvRtspServer::CLiveMediaSource*
	CLvRtspServer::CLiveMediaSource::createNew( UsageEnvironment& env )
{
	return new CLvRtspServer::CLiveMediaSource( env );
}

CLvRtspServer::CLiveMediaSource::CLiveMediaSource( UsageEnvironment& env )
	: FramedSource( env )
{
}

CLvRtspServer::CLiveMediaSource::~CLiveMediaSource()
{
}

void CLvRtspServer::CLiveMediaSource::doGetNextFrame()
{
oexEcho( oexMks( " === ", __FUNCTION__ ).Ptr() );

	if ( 0 )
	{	handleClosure(this);
		return;
	} // end if
}

void CLvRtspServer::CLiveMediaSource::deliverFrame()
{
oexEcho( oexMks( " === ", __FUNCTION__ ).Ptr() );

	// Are we waiting for data?
	if ( !isCurrentlyAwaitingData() )
		return;

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
    if ( !pSms->addSubsession( CLiveMediaSubsession::createNew( *m_pEnv, True ) ) )
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
