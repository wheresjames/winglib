// lv_rtsp_client.h

class CLvRtspClient : oex::CThread
{
public:

	class CVideoSink : public MediaSink
	{
	public:

		enum
		{
            eDefaultBufferSize = 2000000
		};

	public:

		/// Constructor
		CVideoSink( UsageEnvironment &rEnv );
		virtual ~CVideoSink();

		/// Returns non-zero if new frame is needed
		int needFrame();

		/// Reads frame info
		int LockFrame( sqbind::CSqBinary *dat, int to );

		/// Releases the frame buffer
		int UnlockFrame();

		/// Sets the packet header
		void setHeader( sqbind::CSqBinary *header );

		/// Grabs next frame
		virtual Boolean continuePlaying();

		/// Returns the presentation time
		oex::oexTime& getPts() { return m_ts; }

		/// Returns the Dts time
		oex::oexTime& getDts() { return m_ds; }

	private:

		/// Destructor
//		virtual ~CVideoSink();

	private:

		static void _afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
										struct timeval Pts, unsigned DtsInMicroseconds );

		/// Processes new frames
		void afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
								struct timeval Pts, unsigned DtsInMicroseconds );

	private:

		/// Packet header to add
		sqbind::CSqBinary		m_header;

		/// Data buffer
		sqbind::CSqBinary		m_buf;

		/// Non-zero when a new frame is ready
		int						m_nFrameReady;

		/// Non-zero when a new frame is being captured
		int						m_nFrameGrabbing;

		/// Data lock
		oexLock					m_lock;
		
		/// Timestamp
		oex::oexTime			m_ts;
		
		/// Dts
		oex::oexTime			m_ds;

	};

//	class CAudioSink : public MPEG4LATMAudioRTPSink 
	class CAudioSink : public MediaSink
	{
	public:

		enum
		{
            eDefaultBufferSize = 2000000
		};

	public:

		/// Constructor
		CAudioSink( UsageEnvironment &rEnv );
		virtual ~CAudioSink();

		/// Returns non-zero if new frame is needed
		int needFrame();

		int LockFrame( sqbind::CSqBinary *dat, int to );

		/// Releases the frame buffer
		int UnlockFrame();

		/// Grabs next frame
		virtual Boolean continuePlaying();

		/// Returns the presentation time
		oex::oexTime& getPts() { return m_ts; }

		/// Returns the Dts time
		oex::oexTime& getDts() { return m_ds; }

	private:

		/// Destructor
//		virtual ~CAudioSink();

	private:

		static void _afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
										struct timeval Pts, unsigned DtsInMicroseconds );

		/// Processes new frames
		void afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
								struct timeval Pts, unsigned DtsInMicroseconds );

	private:

		/// Data buffer
		sqbind::CSqBinary		m_buf;

		/// Non-zero when a new frame is ready
		int						m_nFrameReady;

		/// Non-zero when a new frame is being captured
		int						m_nFrameGrabbing;

		/// Timestamp
		oex::oexTime			m_ts;
		
		/// Dts
		oex::oexTime			m_ds;

	};


public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CLvRtspClient )
	_SQBIND_CLASS_CTOR_END( CLvRtspClient )

	/// Default constructor
	CLvRtspClient();

	/// Destructor
	virtual ~CLvRtspClient() { Destroy(); }

	/// Copy
	CLvRtspClient( const CLvRtspClient &r ) { }
	CLvRtspClient& operator = ( const CLvRtspClient &r ) { return *this; }

	/** \addtogroup CLvRtspClient
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Open link
	int Open( const sqbind::stdString &sUrl, int bVideo, int bAudio, sqbind::CSqMulti *m );

	/// Thread open
	int ThreadOpen( const sqbind::stdString &sUrl, int bVideo, int bAudio, sqbind::CSqMulti *m );

	/// Reads frame data, good until UnlockFrame() is called
	int LockVideo( sqbind::CSqBinary *dat, int to );

	/// Releases the frame buffer
	int UnlockVideo();

	/// Reads frame data, good until UnlockFrame() is called
	int LockAudio( sqbind::CSqBinary *dat, int to );

	/// Releases the frame buffer
	int UnlockAudio();

	/// Returns video width
	int getWidth()
	{	return m_width;
	}

	/// Returns video width
	int getHeight()
	{	return m_height;
	}

	/// Returns video width
	int getFps()
	{	return m_fps;
	}

	/// Returns the sdp object
	sqbind::CSqMulti getSDP()
	{	return m_mSdp;
	}

	/// Returns a single sdp value
	sqbind::CSqMulti getSDPValue( const sqbind::stdString &k )
	{	return m_mSdp[ k ];
	}

	/// Returns non-zero if a container is open
	int isOpen()
	{	return m_pRtspClient ? 1 : 0;
	}

	/// Return the frame count
	int getFrameCount()
	{	return m_nFrames;
	}

	/// Returns the video codec name
	sqbind::stdString getVideoCodecName()
	{	return m_sVideoCodec; }

	/// Returns the audio codec name
	sqbind::stdString getAudioCodecName()
	{	return m_sAudioCodec; }

	/// waits for the application to initialize
	int waitInit( int nMax )
	{	return !GetInitEvent().Wait( nMax ); }

	/// Returns non-zero if video was enabled and the open stream contains video
	int isVideo()
	{	return m_bVideo; }

	/// Returns non-zero if audio was enabled and the open stream contains audio
	int isAudio()
	{	return m_bAudio; }

	/// Returns a pointer to the any extra codec data
	sqbind::CSqBinary getExtraVideoData() { return m_extraVideo; }

	/// Returns a pointer to the any extra codec data
	sqbind::CSqBinary getExtraAudioData() { return m_extraAudio; }

	/// Sets the video header
	void setVideoHeader( sqbind::CSqBinary *header )
	{	if ( m_pVs ) m_pVs->setHeader( header );
	} // end

	/// Signal start
	void Play() { m_evtPlay.Signal(); }

	/// Set to non-zero to enable streaming over tcp
	void setStreamOverTCP( int b ) { m_bStreamOverTCP = b; }

	/// Returns non-zero to enable streaming over tcp
	int getStreamOverTCP() { return m_bStreamOverTCP; }

	/// Set to nonzero http port to use for tunneling
	void setTunnelOverHTTPPort( int p ) { m_nTunnelOverHTTPPort = p; }

	/// Returns the port used for HTTP tunneling
	int getTunnelOverHTTPPort() { return m_nTunnelOverHTTPPort; }

	/// Returns the number of audio channels
	int getNumAudioChannels() { return m_nAudioNumChannels; }
	
	/// Returns the audio sample rate
	int getAudioSampleRate() { return m_nAudioRate; }

	/// Returns the audio bits per sample
	int getAudioBps() { return m_nAudioBps; }

	/// Returns the video presentation time seconds	
	int getVideoPtsSec() { return m_pVs ? m_pVs->getPts().tv_sec : 0; }

	/// Returns the video presentation time useconds	
	int getVideoPtsUSec() { return m_pVs ? m_pVs->getPts().tv_usec : 0; }

	/// Returns the video presentation time in useconds	
	SQInteger getVideoPts() { return m_pVs ? oex::oexGetUSecs( m_pVs->getPts() ) : 0; }
	
	/// Returns the audio presentation time seconds	
	int getAudioPtsSec() { return m_pAs ? m_pAs->getPts().tv_sec : 0; }

	/// Returns the audio presentation time useconds	
	int getAudioPtsUSec() { return m_pAs ? m_pAs->getPts().tv_usec : 0; }

	/// Returns the audio presentation time in useconds	
	SQInteger getAudioPts() { return m_pAs ? oex::oexGetUSecs( m_pAs->getPts() ) : 0; }
	
	/// Returns the video Dts time in useconds	
	SQInteger getVideoDts() { return m_pVs ? oex::oexGetUSecs( m_pVs->getDts() ) : 0; }
	
	/// Returns the audio Dts time in useconds	
	SQInteger getAudioDts() { return m_pAs ? oex::oexGetUSecs( m_pAs->getDts() ) : 0; }
	
	/** @} */

	/// Idle processing static function
	static void _OnIdle( void *pData );

	/// Idle processing
	void OnIdle();

protected:

	/// Initializes thread
	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData );

	/// Runs the thread
	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData );

	/// Cleanup
	virtual oex::oexINT EndThread( oex::oexPVOID x_pData );

	/// Thread cleanup
	void ThreadDestroy();

	/// Initializes video stream
	int InitVideo( MediaSubsession *pss );

	/// Initializes video stream
	int InitAudio( MediaSubsession *pss );

private:

	/// Flag to end loop
	char					m_nEnd;

	/// Url to open
	sqbind::stdString		m_sUrl;

	/// Non-zero if video should be processed
	int						m_bVideo;

	/// Non-zero if audio should be processed
	int						m_bAudio;

	/// The name of the video codec
	sqbind::stdString		m_sVideoCodec;

	/// The name of the video codec
	sqbind::stdString		m_sAudioCodec;

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

	/// Video sub session
	MediaSubsession			*m_pVsPss;

	/// Audio sink
	CAudioSink				*m_pAs;

	/// Audio sub session
	MediaSubsession			*m_pAsPss;

	/// Left over packet data
	sqbind::CSqBinary		m_buf;

	/// Extra video codec data
	sqbind::CSqBinary		m_extraVideo;

	/// Extra audio codec data
	sqbind::CSqBinary		m_extraAudio;

	/// Number of frames that have been processed
	int						m_nFrames;

	/// Video frame width
	int						m_width;

	/// Video frame height
	int						m_height;

	/// Video frame height
	int						m_fps;
	
	/// Number of audio channels
	int						m_nAudioNumChannels;
	
	/// Audio sample rate
	int						m_nAudioRate;
	
	/// Bits per sample
	int						m_nAudioBps;

	/// Signal to start RTSP stream
	oexEvent				m_evtPlay;

	/// SDP string
	sqbind::CSqMulti		m_mSdp;

	/// Non-zero to stream over TCP
	int						m_bStreamOverTCP;

	/// Non-zero to specify the HTTP port used to tunnel
	int						m_nTunnelOverHTTPPort;
};

