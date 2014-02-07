// lv_rtsp_server.cpp

#include "stdafx.h"
//#include <stdio.h>

#include "RTSPCommon.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"

// CLvRtspServer
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspServer, CLvRtspServer )

	SQBIND_MEMBER_FUNCTION( CLvRtspServer, StartServer )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, getParam )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, getParamStr )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, setParam )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, setParamStr )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, SetHttpTunnelingPort )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, isRunning )
//	SQBIND_MEMBER_FUNCTION( CLvRtspServer,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspServer,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspServer,  )

SQBIND_REGISTER_CLASS_END()

void CLvRtspServer::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CLvRtspServer );
}


//------------------------------------------------------------------
// *** CClientSession
//------------------------------------------------------------------

CClientSession::CClientSession( CLvRtspServer *pServer, RTSPServer *pRtspServer,
								unsigned sessionId, int clientSocket, struct sockaddr_in clientAddr )
	: m_pServer( pServer ),
	  RTSPServer::RTSPClientSession( *pRtspServer, sessionId, clientSocket, clientAddr )
{_STT();

	// Wrap socket handle
	oex::os::CIpSocket socket( (oex::os::CIpSocket::t_SOCKET)clientSocket, oex::oexFALSE );

	// Parse the url
	m_sId = sqbind::oex2std( socket.PeerAddress().GetId() );

	// Root key
	m_sRoot = sqbind::stdString( oexT( "streams." ) ) + m_sId;

	// We're connected
	if ( m_sRoot.length() )
		m_pServer->setParamStr( m_sRoot + oexT( ".connected" ), oexT( "1" ) ),
		m_pServer->setParamStr( m_sRoot + oexT( ".remote" ), sqbind::oex2std( socket.PeerAddress().GetDotAddress() ) );

}

CClientSession::~CClientSession()
{_STT();

	// Clear stream information
	if ( m_sRoot.length() )
		m_pServer->setParam( m_sRoot, 0 ),
		m_sRoot.clear();

	// Remove our media session
	if ( m_sId.length() )
		m_pServer->Server()->removeServerMediaSession( oexStrToMbPtr( m_sId.c_str() ) );
}

void CClientSession::handleCmd_DESCRIBE( char const *cseq, char const *urlPreSuffix,
										 char const *urlSuffix, char const *fullRequestStr )
{_STT();

	// Sanity checks, (I blame live555)
	if ( !m_pServer || !m_pServer->isServer() )
		return;

	oex::CStr8 sFull( oex::CStr8( m_pServer->Server()->rtspURLPrefix() ) << urlSuffix );
	_STT_SET_TAG( oexMbToStr( sFull ) );

	// Parse the url
	oex::CPropertyBag pb = oex::os::CIpAddress::ParseUrl( oexMbToStrPtr( sFull.Ptr() ) );

	// Stream Name
	m_sName = sqbind::oex2std( oexUrlDecode( pb[ oexT( "path" ) ].ToString().LTrim( oexT( "/" ) ) ) );

	// Read default source information
	m_mParams = m_pServer->getParam( sqbind::stdString( oexT( "sources.") ) + m_sName );

	// Decode GET params
	sqbind::CSqMulti m; m.urldecode( sqbind::oex2std( pb[ oexT( "extra" ) ].ToString() ) );
	m_mParams.mmerge( &m );

	// Use ffmpeg MPEG4 codec
	sqbind::stdString sCodec = m_mParams[ oexT( "codec" ) ].str();
	if ( sCodec == oexT( "MPG4" ) || sCodec == oexT( "MP42" ) || sCodec == oexT( "MPEG4" ) )
		m_mParams[ oexT( "codec" ) ].set( oexT( "FMP4" ) );

	// Add stream info
	if ( m_sRoot.length() )
		m_pServer->setParamStr( m_sRoot + oexT( ".name" ), m_sName ),
		m_pServer->setParamStr( m_sRoot + oexT( ".share" ), m_sId ),
		m_pServer->setParam( m_sRoot + oexT( ".params" ), &m_mParams );

	// Stream description
	sqbind::stdString sDesc = m_pServer->getParamStr( sqbind::stdString( m_sName ) + oexT( ".desc" ) );
	if ( !sDesc.length() )
		sDesc = m_sName;

	// Create media session
	ServerMediaSession* pSms = ServerMediaSession::createNew( *m_pServer->Env(), oexStrToMbPtr( m_sId.c_str() ), oexStrToMbPtr( m_sName.c_str() ), oexStrToMbPtr( sDesc.c_str() ) );
	if ( !pSms )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::createNew() failed : " ), oexMbToStrPtr( m_pServer->Env()->getResultMsg() ) ) );
		return;
	} // end if

	// Add subsession
	CMediaSubsession *pLms = new CMediaSubsession( *m_pServer->Env(), False, m_pServer, m_sId, m_sName, &m_mParams );
	if ( !pLms || !pSms->addSubsession( pLms ) )
	{	oexERROR( 0, oexMks( oexT( "ServerMediaSession::addSubsession() failed : " ), oexMbToStrPtr( m_pServer->Env()->getResultMsg() ) ) );
		return;
	} // end if

	// Add session option to server
	m_pServer->Server()->addServerMediaSession( pSms );

	// Call the base class
	RTSPServer::RTSPClientSession
		::handleCmd_DESCRIBE( cseq, urlPreSuffix, oexStrToMbPtr( m_sId.c_str() ), fullRequestStr );
}

void CClientSession::handleCmd_SETUP( char const* cseq, char const* urlPreSuffix,
									  char const* urlSuffix, char const* fullRequestStr )
{_STT();

	RTSPServer::RTSPClientSession
		::handleCmd_SETUP( cseq, urlPreSuffix, urlSuffix, fullRequestStr );

}

void CClientSession::handleCmd_withinSession( char const* cmdName,
											  char const* urlPreSuffix, char const* urlSuffix,
											  char const* cseq, char const* fullRequestStr )
{_STT();

	RTSPServer::RTSPClientSession
		::handleCmd_withinSession( cmdName, urlPreSuffix, urlSuffix, cseq, fullRequestStr );

}

void CClientSession::handleCmd_PLAY( ServerMediaSubsession* subsession, char const* cseq,
									 char const* fullRequestStr )
{_STT();

	RTSPServer::RTSPClientSession
		::handleCmd_PLAY( subsession, cseq, fullRequestStr );
}

//------------------------------------------------------------------
// *** CRtspServer
//------------------------------------------------------------------

CRtspServer* CRtspServer::createNew( CLvRtspServer *pServer, UsageEnvironment& env, Port ourPort,
									 UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds )
{_STT();
	int ourSocket = setUpOurSocket( env, ourPort );
	if ( ourSocket == -1 )
		return 0;

	return new CRtspServer( pServer, env, ourSocket, ourPort, authDatabase, reclamationTestSeconds );
}

CRtspServer::CRtspServer( CLvRtspServer *pServer, UsageEnvironment& env, int ourSocket, Port ourPort,
						  UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds )
	: m_pServer( pServer ),
	  RTSPServerSupportingHTTPStreaming( env, ourSocket, ourPort, authDatabase, reclamationTestSeconds )
{_STT();
}

CRtspServer::~CRtspServer()
{_STT();
}

RTSPServer::RTSPClientSession*
	CRtspServer::createNewClientSession( unsigned sessionId, int clientSocket, struct sockaddr_in clientAddr )
{_STT();
	return new CClientSession( m_pServer, this, sessionId, clientSocket, clientAddr );
}


ServerMediaSession* CRtspServer::lookupServerMediaSession( char const* streamName )
{_STT();
	return RTSPServer::lookupServerMediaSession( streamName );
}

//------------------------------------------------------------------
// *** CMediaSource
//------------------------------------------------------------------

CMediaSource::CMediaSource( UsageEnvironment& env, CLvRtspServer *pServer,
							const sqbind::stdString &sId, const sqbind::stdString &sName, sqbind::CSqMulti *m )
	: m_pVs( 0 ), m_pServer( pServer ), m_sId( sId ), m_sName( sName ), m_mParams( m ),
	  m_nNeedFrame( 0 ),
	  m_sRoot( sqbind::stdString( oexT( "streams." ) ) + sId ),
	  FramedSource( env )
{_STT();
	m_nBytes = 0;
	m_nFrames = 0;
	m_nSync = 0;

	// Look for user preferences on nal headers
	if ( m )
	{
		if ( m->isset( oexT( "no_nal_strip" ) ) )
			m_bStripNal = 0;

		else if ( m->isset( oexT( "force_nal_strip") ) )
			m_bStripNal = 1;

		else 
			m_bStripNal = ( (*m)[ oexT( "codec" ) ].str() == oexT( "H264" ) ) ? 1 : 0;

	} // end if

	// No nal stripping
	else
		m_bStripNal = 0;
}


CMediaSource::~CMediaSource()
{_STT();
	// Clear stream information
	if ( m_sRoot.length() )
	{
		// Remove processing from server
		if ( m_pServer )
			m_pServer->RemoveSession( m_sRoot );

		m_sRoot.clear();

	} // end if

	// Ditch any video share
	if ( m_pVs )
		delete m_pVs, m_pVs = 0;

	// No more server
	m_pServer = 0;
}

void CMediaSource::doGetNextFrame()
{_STT();

	// We need a frame
	m_nNeedFrame++;

	// Do we have a share open?
	if ( m_pVs )
		return;

	// Must have a server pointer
	if ( !m_pServer )
		return;

	// Add stream id
	if ( m_sRoot.length() )
		m_pServer->setParamStr( m_sRoot + oexT( ".run" ), oexT( "1" ) ),

	// Create a new video share
	m_pVs = new sqbind::CSqFifoShare();
	if ( !m_pVs )
	{	m_pServer->setParamStr( m_sRoot + oexT( ".error" ), oexT( "Failed to create share object" ) );
		return;
	} // end if

	// Hook our update function
	m_pServer->AddSession( m_sRoot, this );
}

int CMediaSource::Update()
{_STT();

	// We must have a server and video share object
	if ( !m_pServer || !m_pVs )
		return 0;

	// Do we need a frame of video?
	if ( 0 >= m_nNeedFrame )
		return 1;

	// Is the share open?
	if ( !m_pVs->isOpen() )
	{
		// Grab the share name
		sqbind::stdString sShare = m_pServer->getParamStr( m_sRoot + oexT( ".share" ) );

		// Attempt to open share
		if ( !m_pVs->Open( sShare ) )
		{	m_pServer->setParamStr( m_sRoot + oexT( ".status" ), sqbind::stdString( oexT( "Waiting for memory share : " ) ) + sShare );
			return m_pServer->getParamStr( m_sRoot + oexT( ".quit" ) ).length() ? 0 : 1;
		} // end if

	} // end if

	// Data to read?
	if ( !m_pVs->isRead() )
		return 1;

	// Attempt to read frame data
	sqbind::CSqBinary data = m_pVs->ReadData();
	m_pVs->incReadPtr();

	// Punt if null frame
	if ( !data.getUsed() )
		return 1;

	if ( !m_nFrames )
		m_pServer->setParamStr( m_sRoot + oexT( ".status" ), oexT( "Running" ) );

	// Accumulate totals
	m_nFrames++;
	m_nBytes += data.getUsed();

	// Timer tick
	oex::oexINT64 nTime = oexGetUnixTime();
	if ( m_nSync != nTime )
	{
		// Updating once per second
		m_nSync = nTime;

		// Check for cancel
		if ( m_pServer->getParamStr( m_sRoot + oexT( ".quit" ) ).length() )
		{	fFrameSize = 0; fTo = 0;
			fPresentationTime.tv_sec = 0;
			fPresentationTime.tv_usec = 0;
			afterGetting( this );
			return 0;
		} // end if

		// Update status
		m_pServer->setParamStr( m_sRoot + oexT( ".frames" ), sqbind::oex2std< oex::CStr >( m_nFrames ) );
		m_pServer->setParamStr( m_sRoot + oexT( ".bytes" ), sqbind::oex2std< oex::CStr >( m_nBytes ) );

	} // end if

	// How much data to send?
	fFrameSize = data.getUsed();
	if ( fFrameSize > fMaxSize )
	{	fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	} // end if
	else
		fNumTruncatedBytes = 0;

	// Copy the data
	// +++ anyway to get out of this???
//	fTo = (unsigned char*)data._Ptr();

	// Skip NAL header if needed
	if ( m_bStripNal && 4 < fFrameSize )
	{
		oex::oexINT nSkip = 0;
		unsigned char *p = (unsigned char*)data.Ptr();

		// Look for NAL header
		if ( 0 == p[ 0 ] && 0 == p[ 1 ] )
		{	if ( 0x01 == p[ 2 ] )
				nSkip = 3;
			else if ( 0x01 == p[ 3 ] )
				nSkip = 4;
		} // end if

		// Copy the data
		fFrameSize -= nSkip;
		oexMemCpy( fTo, &p[ nSkip ], fFrameSize );

	} // end if

	else
		oexMemCpy( fTo, data.Ptr(), fFrameSize );

#if defined( oexDEBUG )
//			oexSHOW( fFrameSize );
//			oexEcho( oexBinToAsciiHexStr( fTo, fFrameSize, 16, 16 ).Ptr() );
#endif

	double fps = 15;
	double dFps = ( 0 < fps && 120 > fps ) ? (double)fps : 30;
	fDurationInMicroseconds = (unsigned int)( double( 1000000 ) / dFps );

	// Use current time on first frame
	if ( fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0 )
		gettimeofday( &fPresentationTime, NULL );

	// Just add the duration
	else
	{	fPresentationTime.tv_usec += fDurationInMicroseconds;
		fPresentationTime.tv_sec += fPresentationTime.tv_usec / 1000000;
		fPresentationTime.tv_usec %= 1000000;
	} // end else

	// We sent a frame
	m_nNeedFrame = 0;

	afterGetting( this );

	return 1;
}


//------------------------------------------------------------------
// *** CMediaSubsession
//------------------------------------------------------------------

CMediaSubsession::CMediaSubsession( UsageEnvironment &env, Boolean reuseFirstSource, CLvRtspServer *pServer,
									const sqbind::stdString &sId, const sqbind::stdString &sName, sqbind::CSqMulti *m )
	: m_pServer( pServer ), m_sId( sId ), m_sName( sName ), m_mParams( m ),
	  OnDemandServerMediaSubsession( env, reuseFirstSource )
{_STT();
}

CMediaSubsession::~CMediaSubsession()
{_STT();
}

/// Creates a new frame source instance
FramedSource* CMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{_STT();

	CMediaSource *pMs = new CMediaSource( envir(), m_pServer, m_sId, m_sName, &m_mParams );
	if ( !pMs )
		return 0;

	// What codec are we using?
	sqbind::stdString sCodec = m_mParams[ oexT( "codec" ) ].str();

	// Create a framer for the video stream
	if ( sCodec == oexT( "H264" ) )
		return H264VideoStreamDiscreteFramer::createNew( envir(), pMs );
//		return new H264VideoStreamFramer( envir(), pMs );
	else if ( sCodec == oexT( "MPG1" ) || sCodec == oexT( "MPG2" ) )
		return MPEG1or2VideoStreamDiscreteFramer::createNew( envir(), pMs );
	else
		return MPEG4VideoStreamDiscreteFramer::createNew( envir(), pMs );
}

/// Creates a new sink instance
RTPSink* CMediaSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{_STT();

	// Desired frame rate
	int fps = m_mParams[ oexT( "fps" ) ].toint();
	if ( 0 >= fps || 60 < fps ) fps = 15;

	// What codec are we using?
	sqbind::stdString sCodec = m_mParams[ oexT( "codec" ) ].str();

	// Create proper video sink
	if ( sCodec == oexT( "H264" ) )
		return H264VideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic );
	else if ( sCodec == oexT( "MPG1" ) || sCodec == oexT( "MPG2" ) )
		return MPEG1or2VideoRTPSink::createNew( envir(), rtpGroupsock );
	else
		return MPEG4ESVideoRTPSink::createNew( envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 1000000 / fps );
}


//------------------------------------------------------------------
// *** CLvRtspServer
//------------------------------------------------------------------

CLvRtspServer::CLvRtspServer()
{_STT();
	m_nEnd = 0;
	m_nPort = 0;
	m_pEnv = oexNULL;
	m_pRtspServer = oexNULL;
	m_pUserDb = oexNULL;
	m_nHttpTunnelPort = -1;
}

void CLvRtspServer::Destroy()
{_STT();

	// Signal the thread to stop
	m_nEnd = 1;

	// Stop the thread
	Stop();

	// Reset stop flag
	m_nEnd = 0;

}

int CLvRtspServer::StartServer( int x_nPort )
{_STT();

	// Lose the old server
	Destroy();

	// Must have a valid port number
	if ( 0 >= x_nPort )
	{	setLastErrorStr( oexT( "Invalid port number" ) );
		return 0;
	} // end if

	// Save port
	m_nPort = x_nPort;

	// Start the thread
	if ( Start() )
	{	setLastErrorStr( oexT( "RTSP Server thread failed to start" ) );
		Destroy();
		return 0;
	} // end if

	// Wait for the thing to init
	if ( GetInitEvent().Wait( 3000 ) )
	{	Destroy();
		if ( !getLastErrorStr().length() )
			setLastErrorStr( oexT( "RTSP Server thread failed to initialize in 3 seconds" ) );
		return 0;
	} // end if

	// Ensure we have a valid server pointer
	if ( !m_pRtspServer )
	{	Destroy();
		if ( !getLastErrorStr().length() )
			setLastErrorStr( oexT( "Failed to create RTSP Server object" ) );
		return 0;
	} // end if

	// Did we get a server?
	return 1;
}

void CLvRtspServer::AddSession( const sqbind::stdString &sId, CMediaSource *pMs )
{_STT();

	m_sessions[ sId ] = pMs;
}

void CLvRtspServer::RemoveSession( const sqbind::stdString &sId )
{_STT();

	t_session_map::iterator it = m_sessions.find( sId );
	if ( m_sessions.end() != it )
		m_sessions.erase( it );
}

void CLvRtspServer::UpdateSessions()
{_STT();

	// Update each session
	t_session_map::iterator it = m_sessions.begin();
	while ( m_sessions.end() != it )
		if ( it->second && 0 < it->second->Update() )
			it++;
		else
			m_sessions.erase( it++ );

}

void CLvRtspServer::_OnIdle( void *pData )
{_STT();
	CLvRtspServer *p = (CLvRtspServer*)pData;
	if ( p )
		p->OnIdle();
}

void CLvRtspServer::OnIdle()
{_STT();

	// Sends video to each session
	UpdateSessions();

	// Schedule us to run again
	if ( !m_nEnd )
		m_pEnv->taskScheduler().scheduleDelayedTask( 15000, (TaskFunc*)CLvRtspServer::_OnIdle, this );
}

oex::oexBOOL CLvRtspServer::InitThread( oex::oexPVOID x_pData )
{_STT();

	if ( !ThreadOpen() )
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CLvRtspServer::DoThread( oex::oexPVOID x_pData )
{_STT();

	// Run the thing
	if ( !m_pEnv )
		return oex::oexFALSE;

	// Do the stuff
	m_pEnv->taskScheduler().doEventLoop( &m_nEnd );

	return oex::oexFALSE;
}

oex::oexINT CLvRtspServer::EndThread( oex::oexPVOID x_pData )
{_STT();
	return oex::oexTRUE;
}

void CLvRtspServer::ThreadDestroy()
{_STT();

	if ( m_pRtspServer )
	{	m_pRtspServer->close( m_pRtspServer );
		delete m_pRtspServer;
		m_pRtspServer = oexNULL;
	} // end if

	if ( m_pUserDb )
		m_pUserDb = oexNULL;

	if ( m_pEnv )
		m_pEnv->reclaim(), m_pEnv = oexNULL;
}

int CLvRtspServer::ThreadOpen()
{_STT();

	// Out with the old
	ThreadDestroy();
	setLastErrorStr( oexT( "" ) );

	// Setup usage environment
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	if ( !scheduler )
	{	setLastErrorStr( oexT( "BasicTaskScheduler::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Create the environment
	m_pEnv = BasicUsageEnvironment::createNew( *scheduler );
	if ( !m_pEnv )
	{	setLastErrorStr( oexT( "BasicUsageEnvironment::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Create the server
	m_pRtspServer = CRtspServer::createNew( this, *m_pEnv, m_nPort, m_pUserDb );
	if ( !m_pRtspServer )
	{	setLastErrorStr( oexT( "CRTSPServer::createNew() failed" ) );
		ThreadDestroy();
		return 0;
	} // end if

	// Do we want to enable HTTP tunneling?
	if ( 0 <= m_nHttpTunnelPort )
	{
		// Attempt to enable tunneling on the specified port
		if ( !m_pRtspServer->setUpTunnelingOverHTTP( m_nHttpTunnelPort ? m_nHttpTunnelPort : ( m_nPort + 1 ) ) )
		{	setLastErrorStr( oexT( "CRTSPServer::setUpTunnelingOverHTTP() failed" ) );
			ThreadDestroy();
			return 0;
		} // end if

	} // end if

	// Save the url string
	setParamStr( oexT( "url" ), oexMbToStrPtr( m_pRtspServer->rtspURLPrefix() ) );

	// Set thread name
	_STT_SET_NAME( oexMbToStrPtr( m_pRtspServer->rtspURLPrefix() ) );

	// Remember tunneling port
	setParamStr( oexT( "http_tunneling" ), sqbind::oex2std< oex::CStr >( m_nHttpTunnelPort ? m_nHttpTunnelPort : ( m_nPort + 1 ) ) );

	// Start idle loop
	m_pEnv->taskScheduler().scheduleDelayedTask( 0, (TaskFunc*)CLvRtspServer::_OnIdle, this );

	return oex::oexTRUE;
}

