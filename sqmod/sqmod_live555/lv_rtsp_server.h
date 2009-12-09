// lv_rtsp_server.h

class CLvRtspServer : oex::CThread
{
public:

	class CLiveMediaSubsession : public OnDemandServerMediaSubsession
	{
	public:

		/// Creates an instance of this class
		static CLiveMediaSubsession* createNew( UsageEnvironment& env, Boolean reuseFirstSource );

	private:

		/// Creates a new frame source instance
		virtual FramedSource* createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate );

		/// Creates a new sink instance
		virtual RTPSink* createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource );

	protected:

		/// Constructor
		CLiveMediaSubsession( UsageEnvironment &env, Boolean reuseFirstSource )
			: OnDemandServerMediaSubsession( env, reuseFirstSource )
		{}

		/// Destructor
		virtual ~CLiveMediaSubsession() {}
	};

	class CLiveMediaSource: public FramedSource
	{
	public:
		static CLiveMediaSource* createNew( UsageEnvironment& env );

	protected:

		CLiveMediaSource( UsageEnvironment& env );

		virtual ~CLiveMediaSource();

	private:

		virtual void doGetNextFrame();

		void deliverFrame();

	};

	class CLiveMediaSink: public VideoRTPSink
	{
		public:

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

	/// Announces stream
	static void AnnounceStream( RTSPServer* pRtspServer, ServerMediaSession* pSms, char const* pStreamName );

private:

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
