// lv_rtsp_server.h

class CLvRtspServer;
class CMediaSource: public FramedSource
{
public:

	/// Constructor
	CMediaSource( UsageEnvironment& env, CLvRtspServer *pServer,
				  const sqbind::stdString &sId, const sqbind::stdString &sName, sqbind::CSqMulti *m );

	/// Destructor
	virtual ~CMediaSource();

	/// Call to do source processing
	int Update();

private:

	/// Called when a frame needs delivering
	virtual void doGetNextFrame();

	/// Returns the stream id
	sqbind::stdString getId() const { return m_sId; }

private:

	/// Stream id
	sqbind::stdString		m_sId;
	
	/// Strean name
	sqbind::stdString		m_sName;

	/// Stream params
	sqbind::CSqMulti		m_mParams;

	/// Root info key
	sqbind::stdString		m_sRoot;

	/// Pointer to server
	CLvRtspServer			*m_pServer;

	/// Video share
	sqbind::CSqFifoShare	*m_pVs;

	/// Non-zero when a frame of video is needed
	int						m_nNeedFrame;

	/// Frame count
	oex::oexINT64			m_nFrames;

	/// Bytes sent
	oex::oexINT64			m_nBytes;

	/// Last sync time
	oex::oexINT64			m_nSync;

	/// Skip NAL if needed
	oex::oexINT				m_bH264;

};


class CMediaSubsession : public OnDemandServerMediaSubsession
{
public:

	/// Constructor
	CMediaSubsession( UsageEnvironment &env, Boolean reuseFirstSource, CLvRtspServer *pServer,
					  const sqbind::stdString &sId, const sqbind::stdString &sName, sqbind::CSqMulti *m );

	/// Destructor
	virtual ~CMediaSubsession();

	/// Returns the stream id
	sqbind::stdString getId() const { return m_sId; }

private:

	/// Creates a new frame source instance
	virtual FramedSource* createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate );

	/// Creates a new sink instance
	virtual RTPSink* createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource );

private:

	/// Stream id
	sqbind::stdString		m_sId;

	/// Stream name
	sqbind::stdString		m_sName;

	/// Stream params
	sqbind::CSqMulti		m_mParams;

	/// Pointer to server
	CLvRtspServer			*m_pServer;

};

class CClientSession : public RTSPServer::RTSPClientSession
{
public:

	/// Constructor
	CClientSession( CLvRtspServer *pServer, RTSPServer *pRtspServer,
					unsigned sessionId, int clientSocket, struct sockaddr_in clientAddr );

	/// Destructor
	~CClientSession();

	/// DESCRIBE
	virtual void handleCmd_DESCRIBE( char const *cseq, char const *urlPreSuffix,
									 char const *urlSuffix, char const *fullRequestStr );

	/// SETUP
	virtual void handleCmd_SETUP( char const* cseq, char const* urlPreSuffix,
								  char const* urlSuffix, char const* fullRequestStr );

	/// CMD
	virtual void handleCmd_withinSession( char const* cmdName,
										  char const* urlPreSuffix, char const* urlSuffix,
										  char const* cseq, char const* fullRequestStr );

	/// PLAY
	virtual void handleCmd_PLAY( ServerMediaSubsession* subsession, char const* cseq,
								 char const* fullRequestStr );

private:

	/// Pointer to server
	CLvRtspServer			*m_pServer;

	/// Stream name
	sqbind::stdString		m_sName;

	/// Session id
	sqbind::stdString		m_sId;

	/// Root info key
	sqbind::stdString		m_sRoot;

	/// Request parameters
	sqbind::CSqMulti 		m_mParams;

};

class CLvRtspServer;
class CRtspServer : public RTSPServerSupportingHTTPStreaming
{

public:

	static CRtspServer* createNew( CLvRtspServer *pServer, UsageEnvironment& env, Port ourPort,
								   UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds = 65 );

	/// Constructor
	CRtspServer( CLvRtspServer *pServer, UsageEnvironment& env, int ourSocket, Port ourPort,
				 UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds = 65 );


	/// Destructor
	virtual ~CRtspServer();

protected:

	// Adds media session
	virtual ServerMediaSession* lookupServerMediaSession( char const* streamName );

	/// Creates a new client session
	virtual RTSPClientSession*
		createNewClientSession( unsigned sessionId, int clientSocket, struct sockaddr_in clientAddr );

public:

	/// Pointer to server
	CLvRtspServer			*m_pServer;
};


class CLvRtspServer : oex::CThread
{
public:

	/// Maps session names to session objects
	typedef std::map< sqbind::stdString, CMediaSource* > t_session_map;

public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CLvRtspServer )
	_SQBIND_CLASS_CTOR_END( CLvRtspServer )

	/// Default constructor
	CLvRtspServer();

	/// Destructor
	virtual ~CLvRtspServer() { Destroy(); }

	/// Copy
	CLvRtspServer( const CLvRtspServer &r ) { }
	CLvRtspServer& operator = ( const CLvRtspServer &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CLvRtspServer
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Starts the server
	/**
		@param [in] x_nPort		- TCP port server should use to listen for connections
	*/
	int StartServer( int x_nPort );

	/// Signals that thread loop should exit
	void Quit() { m_nEnd = 1; }

	/// Returns the specified param value
	sqbind::CSqMulti getParam( const sqbind::stdString &sKey )
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return sqbind::CSqMulti();
		sqbind::CSqMulti m;
		m.copy( *m_params.at( sKey ) );
		return m;
	}

	/// Returns the specified param value
	sqbind::stdString getParamStr( const sqbind::stdString &sKey )
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return sqbind::CSqMulti();
		sqbind::stdString s( m_params.at( sKey )->str() );
		return s;
	}

	/// Sets the specified param value
	void setParam( const sqbind::stdString &sKey, sqbind::CSqMulti *m )
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return;
		if ( m )
			m_params.at( sKey )->copy( *m );
		else
			m_params.erase_at( sKey );
	}

	/// Sets the specified param value
	void setParamStr( const sqbind::stdString &sKey, const sqbind::stdString &s )
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return;
		if ( s.length() )
			m_params.at( sKey )->set( s );
		else
			m_params.erase_at( sKey );
	}

	/// Returns the last error string
	sqbind::stdString getLastErrorStr()
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return sqbind::stdString();
		return m_sLastError;
	}

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s )
	{	oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return;
		m_sLastError = s;
	}

	/** @} */

	/// Returns the useage environment object
	UsageEnvironment* Env() { return m_pEnv; }

	/// Adds the specified session to the update list
	void AddSession( const sqbind::stdString &sId, CMediaSource *pMs );

	/// Removes the specified session
	void RemoveSession( const sqbind::stdString &sId );

	/// Returns non-zero if valid server object
	int isServer() { return m_pRtspServer ? 1 : 0; }

	/// Returns a pointer to the live555 derived server object
	CRtspServer* Server() { return m_pRtspServer; }

protected:

	/// Initializes thread
	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData );

	/// Runs the thread
	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData );

	/// Cleanup
	virtual oex::oexINT EndThread( oex::oexPVOID x_pData );

	/// Thread cleanup
	void ThreadDestroy();

	/// Initializes the threaded environment
	int ThreadOpen();

	/// Update streaming sessions
	void UpdateSessions();

	/// Static message callback
	static void _OnIdle( void *pData );

	/// Idle processing
	void OnIdle();

private:

	/// Thread lock
	oexLock									m_lock;

	/// List of session objects
	t_session_map							m_sessions;

	/// Parameters
	sqbind::CSqMulti						m_params;

	/// A string describing the last error
	sqbind::stdString						m_sLastError;

	/// Usage environment
	UsageEnvironment						*m_pEnv;

	/// RTSP server
	CRtspServer								*m_pRtspServer;

	/// User authentication object
	UserAuthenticationDatabase				*m_pUserDb;

	/// Server port
	int										m_nPort;

	/// Non-zero if thread should stop
	char									m_nEnd;
};

