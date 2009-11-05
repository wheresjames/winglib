// lv_rtsp_client.h

class CLvRtspClient : oex::CThread
{
public:

	class CVideoSink : public MediaSink
	{
	public:

		enum
		{
			eDefaultBufferSize = 200000
		};

	public:

		/// Constructor
		CVideoSink( UsageEnvironment &rEnv );

		/// Destructor
		virtual ~CVideoSink();

		/// Reads frame info
		int LockFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

		/// Releases the frame buffer
		int UnlockFrame();

	private:

		/// Grabs next frame
		virtual Boolean continuePlaying();

	private:

		static void _afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
										struct timeval presentationTime, unsigned durationInMicroseconds );

		/// Processes new frames
		void afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
								struct timeval presentationTime, unsigned durationInMicroseconds );

	private:

		/// Data buffer
		sqbind::CSqBinary		m_buf;

		/// Signals when a new frame is ready
		oex::CSignal			m_sigFrame;

	};


public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CLvRtspClient )
	_SQBIND_CLASS_CTOR_END( CLvRtspClient )

	/// Default constructor
	CLvRtspClient();

	/// Destructor
	virtual ~CLvRtspClient() { Destroy(); }

	/// Destroy connection
	void Destroy();

	/// Open link
	int Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m );

	/// Thread open
	int ThreadOpen( const sqbind::stdString &sUrl, sqbind::CSqMulti *m );

	/// Reads frame data, good until UnlockFrame() is called
	int LockFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Releases the frame buffer
	int UnlockFrame();

	/// Returns video width
	int getWidth()
	{	return 0;
	}

	/// Returns video width
	int getHeight()
	{	return 0;
	}

	/// Returns non-zero if a container is open
	int isOpen()
	{	return m_pRtspClient ? 1 : 0;
	}

	/// Return the frame count
	int getFrameCount()
	{	return m_nFrames;
	}

	/// Returns the codec name
	sqbind::stdString getCodecName()
	{	return m_sVideoCodec; }

	/// waits for the application to initialize
	int waitInit( int nMax )
	{	return !GetInitEvent().Wait( nMax ); }

protected:

	/// Initializes thread
	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData );

	/// Runs the thread
	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData );

	/// Cleanup
	virtual oex::oexINT EndThread( oex::oexPVOID x_pData );

	/// Thread cleanup
	void ThreadDestroy();

private:

	/// Flag to end loop
	char					m_nEnd;

	/// Url to open
	sqbind::stdString		m_sUrl;

	/// The name of the video codec
	sqbind::stdString		m_sVideoCodec;

	/// Parameters
	sqbind::CSqMulti		m_mParams;

	/// Usage environment
	UsageEnvironment		*m_pEnv;

	/// RTSP client object
	RTSPClient				*m_pRtspClient;

	/// Session object
	MediaSession			*m_pSession;

	/// Video sink
	CVideoSink				*m_pVs;

	/// Left over packet data
	sqbind::CSqBinary		m_buf;

	/// Number of frames that have been processed
	int						m_nFrames;

};
