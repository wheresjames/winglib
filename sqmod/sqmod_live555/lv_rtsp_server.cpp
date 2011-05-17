// lv_rtsp_server.cpp

#include "stdafx.h"

#include "GroupsockHelper.hh"

// CLiveMediaSubsession::createNew
CLvRtspServer::CLiveMediaSubsession* CLvRtspServer::CLiveMediaSubsession::createNew( UsageEnvironment& env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer, ServerMediaSession *pSms, sqbind::CSqMulti *mParams )
{_STT();
	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSubsession( env, reuseFirstSource, pRtspServer, pSms, mParams );
}

// CLiveMediaSubsession::createNewStreamSource
FramedSource* CLvRtspServer::CLiveMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{_STT();
	if ( !m_pRtspServer )
		return oexNULL;

	m_pSource = CLvRtspServer::CLiveMediaSource::createNew( envir(), m_pRtspServer, &m_p );

	estBitrate = 500;

	// Create a framer for the video stream
//	if ( m_p[ oexT( "codec" ) ].str() == oexT( "H264" ) )
//		return H264VideoStreamDiscreteFramer::createNew( envir(), m_pSource );
//		return new H264VideoStreamFramer( envir(), m_pSource );
//	else if ( m_p[ oexT( "codec" ) ].str() == oexT( "MPG1" ) || m_p[ oexT( "codec" ) ].str() == oexT( "MPG2" ) )
//		return MPEG1or2VideoStreamDiscreteFramer::createNew( envir(), m_pSource );
//	else
		return MPEG4VideoStreamDiscreteFramer::createNew( envir(), m_pSource );
}

// CLiveMediaSubsession::createNewRTPSink
RTPSink* CLvRtspServer::CLiveMediaSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{_STT();

	// Get frame rate
	long fps = m_p[ oexT( "fps" ) ].toint();
	if ( 0 >= fps || 100 < fps ) fps = 15;

	// Create proper video sink
//	if ( m_p[ oexT( "codec" ) ].str() == oexT( "H264" ) )
//		return H264VideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000000 / fps );
//	else if ( m_p[ oexT( "codec" ) ].str() == oexT( "MPG1" ) || m_p[ oexT( "codec" ) ].str() == oexT( "MPG2" ) )
//		return MPEG1or2VideoRTPSink::createNew( envir(), rtpGroupsock );
//	else
		return MPEG4ESVideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000000 / fps );
}

int CLvRtspServer::CLiveMediaSubsession::queueFrame( oex::CBin *pFrame, int fps )
{
	if ( m_pSource )
		m_pSource->queueFrame( pFrame, fps );

	return 1;
}

// CLiveMediaSource::createNew()
CLvRtspServer::CLiveMediaSource* CLvRtspServer::CLiveMediaSource::createNew( UsageEnvironment& env, CLvRtspServer *pRtspServer, sqbind::CSqMulti *mParams )
{_STT();

	if ( !pRtspServer )
		return oexNULL;

	return new CLvRtspServer::CLiveMediaSource( env, pRtspServer, mParams );
}


// CLiveMediaSource::doGetNextFrame()
void CLvRtspServer::CLiveMediaSource::doGetNextFrame()
{_STT();

	// Need another frame
	m_bFrameReady = 0;
}

int CLvRtspServer::CLiveMediaSource::queueFrame( oex::CBin *pFrame, int fps )
{
	// Terminate stream?
	if ( !pFrame || !pFrame->getUsed() )
	{	handleClosure( this );
		return 0;
	} // end if

	// Must use previous frame first
	// +++ May need frame buffering
	if ( m_bFrameReady )
		return 0;

	// Queue frame
	m_frame = *pFrame;
	m_bFrameReady = 1;

	deliverFrame( &m_frame, fps );

	return 1;
}


// CLiveMediaSource::deliverFrame()
void CLvRtspServer::CLiveMediaSource::deliverFrame( oex::CBin *pFrame, int fps )
{_STT();

	// ???
	if ( !fTo )
		return;

	// End of stream?
	if ( !pFrame || !pFrame->getUsed() )
	{	fFrameSize = 0; fTo = 0;
		if ( m_pRtspServer )
			m_pRtspServer->Quit();
//		else
//			handleClosure(this);
		fPresentationTime.tv_sec = 0;
		fPresentationTime.tv_usec = 0;
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

	double dFps = ( 0 < fps && 120 > fps ) ? (double)fps : 30;
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

	// +++ This one crashes, I don't quite get the threading issues here...
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
//	m_fDeliverFrame = oexNULL;
	m_pUserData = oexNULL;
	m_nPort = 0;
}

void CLvRtspServer::Destroy()
{_STT();
	// Tell thread loop to exit
	m_nEnd = 1;

	// Stop the thread
	Stop();

	m_nEnd = 0;
	m_nFrames = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
	m_nPort = 0;
}

int CLvRtspServer::StartServer( int x_nPort )
{_STT();

	// Lose the old server
	Destroy();

	// Must have a non-zero port number
	if ( !x_nPort )
		return 0;

	// Save port
	m_nPort = x_nPort;

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
	if ( !ThreadOpen( ) )
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

int CLvRtspServer::ThreadOpen()
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

	// Buffer size
//	OutPacketBuffer::maxSize = 2000000;

		// Create the RTSP server:
	m_pRtspServer = RTSPServer::createNew( *m_pEnv, m_nPort, pUad );
	if ( !m_pRtspServer )
	{	oexERROR( 0, oexMks( oexT( "RTSPServer::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Prevents race condition with queue
	m_pEnv->taskScheduler().scheduleDelayedTask( 0, (TaskFunc*)CLvRtspServer::_ProcessMsgs, this );

	return 1;
}

int CLvRtspServer::Msg( const sqbind::stdString &sParams )
{_STT();

	oexAutoLock ll( m_lockMsgs );
	if ( !ll.IsLocked() )
		return 0;

	if ( !m_pEnv )
		return 0;

	m_lstMsgs.push_back( sParams );

	m_evtMsgReady.Signal();

	return 1;
}

void CLvRtspServer::_ProcessMsgs( void *pData )
{_STT();
	CLvRtspServer *p = (CLvRtspServer*)pData;
	if ( p ) p->ProcessMsgs();
}

void CLvRtspServer::ProcessMsgs()
{_STT();

	// While there are messages in the queue
	if ( !m_evtMsgReady.Wait( 0 ) )
		while ( m_lstMsgs.begin() != m_lstMsgs.end() )
		{
			// Message holder
			sqbind::CSqMulti msg;

			{ // Scope

				oexAutoLock ll( m_lockMsgs );
				if ( !ll.IsLocked() )
					return;

				// Get first message
				msg.deserialize( *m_lstMsgs.begin() );

				// Remove this message from the queue
				m_lstMsgs.erase( m_lstMsgs.begin() );

			} // end scope lock
			
			// Process if valid command
			if ( msg.isset( "cmd" ) )
				ProcessMsg( msg[ "cmd" ].str(), &msg );

		} // end while

	// Reset the message signal
	m_evtMsgReady.Reset();

	// Schedule us to run again
	if ( !m_nEnd )
		m_pEnv->taskScheduler().scheduleDelayedTask( 15000, (TaskFunc*)CLvRtspServer::_ProcessMsgs, this );

}

void CLvRtspServer::ProcessMsg( const sqbind::stdString &sCmd, sqbind::CSqMulti *pParams )
{_STT();

	// Sanity check
	if ( !sCmd.length() || !pParams )
		return;

	// CreateStream
	if ( sCmd == oexT( "CreateStream" ) )
		CreateStream( (*pParams)[ "id" ].str(), pParams );

	else if ( sCmd == oexT( "CloseStream" ) )
		CloseStream( (*pParams)[ "id" ].str() );

	// DeliverFrame
	else if ( sCmd == oexT( "DeliverFrame" ) )
		DeliverFrame( (*pParams)[ "stream_id" ].str(), (*pParams)[ "frame_id" ].str(),
					  (*pParams)[ "fps" ].toint() );

}

int CLvRtspServer::CloseStream( const sqbind::stdString &sId )
{_STT();

	if ( !m_pEnv || !m_pRtspServer || !sId.length() )
		return 0;

	// Do we have such a session?
	t_SessionMap::iterator it = m_mapSession.find( sId );
	if ( m_mapSession.end() == it || !it->second )
		return 0;

	// Remove from server
	m_pRtspServer->removeServerMediaSession( it->second->Session() );

	// Drop from the map
	m_mapSession.erase( it );

	return 1;
}

int CLvRtspServer::CreateStream( const sqbind::stdString &sId, sqbind::CSqMulti *pParams )
{_STT();

	if ( !m_pEnv || !m_pRtspServer || !sId.length() )
		return 0;

	// Create media session
	sqbind::stdString sDesc = (*pParams)[ "desc" ].str();
	ServerMediaSession* pSms = ServerMediaSession::createNew( *m_pEnv, sId.c_str(), sId.c_str(), 
															  sDesc.length() ? sDesc.c_str() : sId.c_str() );
	if ( !pSms )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::createNew() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Add subsession
	CLiveMediaSubsession *pLms = CLiveMediaSubsession::createNew( *m_pEnv, True, this, pSms, pParams );
	if ( !pLms || !pSms->addSubsession( pLms ) )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::addSubsession() failed : " ), oexMbToStrPtr( m_pEnv->getResultMsg() ) ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Add session option to server
	m_pRtspServer->addServerMediaSession( pSms );

	// Save session pointer
	m_mapSession[ sId ] = pLms;

	return 1;
}

int CLvRtspServer::DeliverFrame( const sqbind::stdString &sStreamId, const sqbind::stdString &sFrameId, int fps )
{_STT();

	// Sanity checks
	if ( !sStreamId.length() )
		return 0;

	// Do we have such a session?
	t_SessionMap::iterator it = m_mapSession.find( sStreamId );
	if ( m_mapSession.end() == it || !it->second )
		return 0;

	// Check for a valid frame
	oex::CBin binFrame;
	
	// Frame id?
	if ( sFrameId.length() ) 
	{	binFrame = oexGetBin( sFrameId.c_str() );
		if ( !binFrame.getUsed() )
			return 0;
	} // end if

	// Deliver the frame to the session
	it->second->queueFrame( &binFrame, fps );

	return 1;
}

sqbind::stdString CLvRtspServer::getUrl( const sqbind::stdString &sId )
{
	// Sanity checks
	if ( !sId.length() )
		return "";

	// Do we have such a session?
	t_SessionMap::iterator it = m_mapSession.find( sId );
	if ( m_mapSession.end() == it || !it->second )
		return "";

	return it->second->getUrl();
}

int CLvRtspServer::needFrame( const sqbind::stdString &sId )
{
	// Sanity checks
	if ( !sId.length() )
		return 0;

	// Do we have such a session?
	t_SessionMap::iterator it = m_mapSession.find( sId );
	if ( m_mapSession.end() == it || !it->second )
		return 0;

	return it->second->needFrame();
}
