// lv_rtsp_server.h

class CLvRtspServer : oex::CThread
{
public:

	/// Deliver frame function prototype
	typedef void (*f_deliverFrame)( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame );

	/// Parameter object type
	typedef oexStdList( sqbind::stdString )	t_MsgList;

public:

	class CLiveMediaSource: public FramedSource
	{
	public:

		static CLiveMediaSource* createNew( UsageEnvironment& env, CLvRtspServer *pRtspServer, sqbind::CSqMulti *mParams );

		/// Queues a frame
		int queueFrame( oex::CBin *pFrame, int fps );

		/// Returns non-zero if the session needs a frame
		int needFrame() { return !m_bFrameReady; } // { return !m_sigFrameReady.Wait( 0 ) ? 0 : 1; }

	protected:

		CLiveMediaSource( UsageEnvironment& env, CLvRtspServer *pRtspServer, sqbind::CSqMulti *mParams )
			: FramedSource( env ), m_pRtspServer( pRtspServer ), m_p( mParams )
		{
			fTo = 0;
			m_bFrameReady = 0;
		}


		virtual ~CLiveMediaSource()
		{
			m_pRtspServer = oexNULL;
		}

	private:

		virtual void doGetNextFrame();

//		static void _deliverFrame( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame );
		void deliverFrame( oex::CBin *pFrame, int fps );

	private:

		/// Set when a frame is ready
//		oexSignal					m_sigFrameReady;
		int							m_bFrameReady;

		/// Next frame
		oex::CBin					m_frame;

		/// Pointer to the video server
		CLvRtspServer 				*m_pRtspServer;

		/// Params
		sqbind::CSqMulti			m_p;
	};

	class CLiveMediaSubsession : public OnDemandServerMediaSubsession
	{
	public:

		/// Creates an instance of this class
		static CLiveMediaSubsession* createNew( UsageEnvironment& env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer, ServerMediaSession *pSms, sqbind::CSqMulti *mParams );

		/// Queues a frame
		int queueFrame( oex::CBin *pFrame, int fps );

		/// Returns the url string for this item
		sqbind::stdString getUrl() { return m_sUrl; }

		/// Returns non-zero if the session needs a frame
		int needFrame() { if ( !m_pSource ) return 0; return m_pSource->needFrame(); }

		/// Returns pointer to session object
		ServerMediaSession* Session() { return m_pSms; }

	private:

		/// Creates a new frame source instance
		virtual FramedSource* createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate );

		/// Creates a new sink instance
		virtual RTPSink* createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource );

		virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
		{
//oexM();
			return oexNULL;
		}

	protected:

		/// Constructor
		CLiveMediaSubsession( UsageEnvironment &env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer, ServerMediaSession *pSms, sqbind::CSqMulti *mParams )
			: OnDemandServerMediaSubsession( env, reuseFirstSource ),
			  m_pRtspServer( pRtspServer ),
			  m_pSms( pSms ),
			  m_p( mParams )
		{
			m_pSource = oexNULL;

			// Save URL
			if ( m_pRtspServer && m_pRtspServer->RtspServer() && m_pSms )
			{	char* pUrl = m_pRtspServer->RtspServer()->rtspURL( m_pSms );
				if ( pUrl )
				{	m_sUrl = oexMbToStrPtr( pUrl );
					delete [] pUrl;
				} // end if
			} // end if

		}

		/// Destructor
		virtual ~CLiveMediaSubsession()
		{
			m_pSms = oexNULL;
			m_pSource = oexNULL;
			m_pRtspServer = oexNULL;
		}


	private:

		/// Session pointer
		CLiveMediaSource			*m_pSource;

		/// Pointer to the video server
		CLvRtspServer 				*m_pRtspServer;

		/// Pointer to the ServerMediaSession object
		ServerMediaSession			*m_pSms;

		/// The url that can be used to access the stream
		sqbind::stdString			m_sUrl;

		/// Video codec
		sqbind::stdString			m_sVideoCodec;

		/// Params
		sqbind::CSqMulti			m_p;

	};


	/// Parameter object type
	typedef oexStdMap( sqbind::stdString, CLvRtspServer::CLiveMediaSubsession* )	t_SessionMap;

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
	int StartServer( int x_nPort );

public:

	/// Returns non-zero if the server thread is running
	int isThread()
	{	return CThread::IsRunning(); }

public:

	/// Returns the url that can be used to access the string
	sqbind::stdString getUrl( const sqbind::stdString &sId );

	/// Returns non-zero if the specified device needs more video
	int needFrame( const sqbind::stdString &sId );

	/// Signals that message loop should exit
	void Quit() { m_nEnd = 1; }

	/// Sends a message to the thread
	int Msg( const sqbind::stdString &sParams );

	/// Callback that processes messages
	static void _ProcessMsgs( void* pData );

	/// Processes messages in queue
	void ProcessMsgs();

	/// Process single message
	void ProcessMsg( const sqbind::stdString &sCmd, sqbind::CSqMulti *pParams );

	/// Creates a stream on the server
	int CreateStream( const sqbind::stdString &sId, sqbind::CSqMulti *pParams );

	/// Closes the specified stream
	int CloseStream( const sqbind::stdString &sId );

	/// Adds frame to the output stream queue
	int DeliverFrame( const sqbind::stdString &sStreamId, const sqbind::stdString &sFrameId, int fps );

	/** @} */

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


public:

	/// Access rtsp server variable
	RTSPServer* RtspServer() { return m_pRtspServer; }

private:

	/// Message queue
	t_MsgList				m_lstMsgs;

	/// Message list lock
	oexLock					m_lockMsgs;

	/// Session callback queue
	sqbind::CSqMsgQueue		*m_pMsgQueue;

	/// Set when an event is waiting
	oexEvent				m_evtMsgReady;

	/// Next frame to be sent
	sqbind::CSqBinary		m_binFrame;

	/// Session map
	t_SessionMap			m_mapSession;

	/// User data passed to m_fDeliverFrame
	oex::oexPVOID			m_pUserData;

	/// Usage environment
	UsageEnvironment		*m_pEnv;

	/// The RTSP server object
	RTSPServer				*m_pRtspServer;

	/// Server port
	int						m_nPort;

	/// Number of frames processed
	int						m_nFrames;

	/// Non-zero if thread should stop
	char					m_nEnd;
};
