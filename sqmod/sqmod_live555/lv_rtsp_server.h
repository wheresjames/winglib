// lv_rtsp_server.h

class CLvRtspServer : oex::CThread
{
public:

	/// Deliver frame function prototype
	typedef void (*f_deliverFrame)( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame );

public:

	class CLiveMediaSubsession : public OnDemandServerMediaSubsession
	{
	public:

		/// Creates an instance of this class
		static CLiveMediaSubsession* createNew( UsageEnvironment& env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer );

	private:

		/// Creates a new frame source instance
		virtual FramedSource* createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate );

		/// Creates a new sink instance
		virtual RTPSink* createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource );

		virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
		{
oexM();
			return oexNULL;
		}


	protected:

		/// Constructor
		CLiveMediaSubsession( UsageEnvironment &env, Boolean reuseFirstSource, CLvRtspServer *pRtspServer )
			: OnDemandServerMediaSubsession( env, reuseFirstSource ),
			  m_pRtspServer( pRtspServer )
		{
		}

		/// Destructor
		virtual ~CLiveMediaSubsession()
		{
			m_pRtspServer = oexNULL;
		}


	private:

		/// Pointer to the video server
		CLvRtspServer 				*m_pRtspServer;
	};

	class CLiveMediaSource: public FramedSource
	{
	public:
		static CLiveMediaSource* createNew( UsageEnvironment& env, CLvRtspServer *pRtspServer );

	protected:

		CLiveMediaSource( UsageEnvironment& env, CLvRtspServer *pRtspServer )
			: FramedSource( env ), m_pRtspServer( pRtspServer )
		{
		}


		virtual ~CLiveMediaSource()
		{
			m_pRtspServer = oexNULL;
		}

	private:

		virtual void doGetNextFrame();

		static void _deliverFrame( oex::oexPVOID pUserData, sqbind::CSqBinary *pFrame );

		void deliverFrame( sqbind::CSqBinary *pFrame );

	private:

		/// Pointer to the video server
		CLvRtspServer 				*m_pRtspServer;
	};


public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CLvRtspServer )
	_SQBIND_CLASS_CTOR_END( CLvRtspServer )

	/// Default constructor
	CLvRtspServer();

	/// Destructor
	virtual ~CLvRtspServer() { Destroy(); }

	/// Destroy connection
	void Destroy();

	/// Starts the server
	int StartServer( sqbind::CSqMulti *m );

public:

	/// Returns non-zero if the server thread is running
	int isThread()
	{	return CThread::IsRunning(); }

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
	int ThreadOpen( sqbind::CSqMulti *m );

public:

	/// Sets the callback function for initiating more data
	void setServerCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &x_sDoGetNextFrame )
	{	m_pMsgQueue = x_pMsgQueue; m_sDoGetNextFrame = x_sDoGetNextFrame; }

	/// Sets the next frame that will be sent
	void setNextFrame( sqbind::CSqBinary *bin, sqbind::CSqBinary *pFrame )
	{
		if ( bin )
			m_binFrame = *bin;
	}

	/// Call deliver frame function
	void deliverFrame( sqbind::CSqBinary *pFrame )
	{
		// Get reference to frame data
		if ( pFrame && pFrame->getUsed() )
			m_binFrame = *pFrame;
		else
			m_binFrame.Free();

		// Do we have a deliver function
		if ( m_fDeliverFrame )
			m_fDeliverFrame( m_pUserData, &m_binFrame );
	}

	/// Calls into script to get the next frame
	int CallDoGetNextFrame( f_deliverFrame pDeliverFrame, oex::oexPVOID pUserData );

	/// Returns the url that can be used to access the string
	sqbind::stdString getUrl() { return m_sUrl; }

private:

	/// The url that can be used to access the stream
	sqbind::stdString		m_sUrl;

	/// Session callback queue
	sqbind::CSqMsgQueue		*m_pMsgQueue;

	/// Server callback function
	sqbind::stdString		m_sDoGetNextFrame;

	/// Next frame to be sent
	sqbind::CSqBinary		m_binFrame;

	/// Deliver frame callback function
	f_deliverFrame			m_fDeliverFrame;

	/// User data passed to m_fDeliverFrame
	oex::oexPVOID			m_pUserData;

	/// Usage environment
	UsageEnvironment		*m_pEnv;

	/// The RTSP server object
	RTSPServer				*m_pRtspServer;

	/// Server params
	sqbind::CSqMulti		m_mParams;

	/// Number of frames processed
	int						m_nFrames;

	/// Non-zero if thread should stop
	char					m_nEnd;
};
