// sq_freenect.h

class CSqFreenect : oex::CThread
{
public:
		
	enum
	{
		eMaxBuffers = 8
	};

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqFreenect )
	_SQBIND_CLASS_CTOR_END( CSqFreenect )

	/// Default constructor
	CSqFreenect();

	/// Destructor
	virtual ~CSqFreenect();

	/** \addtogroup CSqFreenect
		@{
	*/

	/// Releases resources
	void Destroy();

	/// Initializes output device information
	int Init();

	/// Returns non-zero if freenect has been initialized
	int isInitialized() { return 0 <= m_nInit; }

	/// Returns the number of devices
	int getNumDevices() { return m_nNumDevices; }

	/// Opens the specified device
	void OpenDevice( int i ) { m_nDev[ 0 ] = i; }

	/// Closes any open device
	void CloseDevice() { m_nDev[ 0 ] = -1; }

	/// Returns the index of the open device, 
	/// or less than zero if there is no device open
	int getOpenDevice() { return m_nDev[ 1 ]; }

	/// Sets the level of error logging
	void setLogLevel( int level ) { m_nLogLevel[ 0 ] = level; }

	/// Returns the current log level
	int getLogLevel() { return m_nLogLevel[ 0 ]; }
	
	/// Clears the error log
	void ClearErrorLog() { m_sErrorLog.clear(); }
	
	/// Sets the LED state
	void setLED( int nState ) { m_nLEDState[ 0 ] = nState; }

	/// Gets the current LED state
	int getLED() { return m_nLEDState[ 0 ]; }
	
	/// Sets the tilt angle
	void setTilt( double dAngle ) { m_dTiltState[ 0 ] = dAngle; }
	
	/// Gets the tilt angle
	double getTilt() { return m_dTiltState[ 0 ]; }
	
	/// Returns the video buffer
	sqbind::CSqBinary getVideo( int i ) 
	{	if ( 0 > i || eMaxBuffers <= i ) 
			return sqbind::CSqBinary(); 
		return m_binVideo[ i ]; 
	}

	/// Start streaming rgb image
	void StartVideo( int nFmt ) { m_nVideoFmt[ 0 ] = nFmt; }
	
	/// Returns the current video format
	int getVideoFormat() { return m_nVideoFmt[ 0 ]; }

	/// Stops streaming rgb image
	void StopVideo() { m_nVideoFmt[ 0 ] = -1; }

	/// Returns current video buffer index
	int getVideoFrameIdx() { return m_nVideoIdx; }

	/// Returns the depth buffer
	sqbind::CSqBinary getDepth( int i ) 
	{	if ( 0 > i || eMaxBuffers <= i ) 
			return sqbind::CSqBinary(); 
		return m_binDepth[ i ]; 
	}

	/// Starts depth map streaming
	void StartDepth( int nFmt ) { m_nDepthFmt[ 0 ] = nFmt; }

	/// Returns the current depth format
	int getDepthFormat() { return m_nDepthFmt[ 0 ]; }

	/// Stops depth map streaming
	void StopDepth() { m_nDepthFmt[ 0 ] = -1; }
	
	/// Returns current depth buffer index
	int getDepthFrameIdx() { return m_nDepthIdx; }

	/// Returns the width of a video frame
	int getVideoWidth() { return FREENECT_FRAME_W; }

	/// Returns the height of a video frame
	int getVideoHeight() { return FREENECT_FRAME_H; }

	/// Returns the width of a depth frame
	int getDepthWidth() { return FREENECT_IR_FRAME_W; }

	/// Returns the height of a depth frame
	int getDepthHeight() { return FREENECT_IR_FRAME_H; }

	/** @} */

private:

	/// Callback proxy
	static void _LogCallback( freenect_context *dev, freenect_loglevel level, const char *msg );
	
	/// Log callback
	void LogCallback( freenect_loglevel level, const char *msg );
	
	/// Proxy called when there is new depth information
	static void _OnDepth( freenect_device *dev, void *depth, uint32_t timestamp );
	
	/// Called when there is new depth information
	void OnDepth( void *depth, uint32_t timestamp );
	
	/// Proxy called when there is new video information
	static void _OnVideo( freenect_device *dev, void *rgb, uint32_t timestamp );

	/// Called when there is new video information
	void OnVideo( void *rgb, uint32_t timestamp );

	/// Get raw acceleration?
	sqbind::CSq3dVector getRawAccel();
	
	/// Get some other kind of acceleration?
	sqbind::CSq3dVectord getMksAccel();
	
protected:

	/// Initialize streaming rgb image
	int ThreadInitVideo();
	
	/// Initializes depth map streaming
	int ThreadInitDepth();

	/// Update device
	int ThreadUpdate();

	/// Opens a device
	int ThreadOpenDevice( int i );

	/// Closes any open device
	int ThreadCloseDevice();

	/// Initializes thread
	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData );

	/// Runs the thread
	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData );

	/// Cleanup
	virtual oex::oexINT EndThread( oex::oexPVOID x_pData );

	/// Thread cleanup
	void ThreadDestroy();

private:


	/// Initialization status
	int						m_nInit;

	/// Number of devices
	int						m_nNumDevices;

	/// Open device index
	int						m_nDev[ 2 ];

	/// freenect context
	freenect_context		*m_pfc;

	/// freenect device
	freenect_device			*m_pfd;
	
	/// Error string
	sqbind::stdString		m_sErrorLog;

	/// The video format to use
	int						m_nVideoFmt[ 2 ];

	/// Video buffer
	sqbind::CSqBinary		m_binVideo[ eMaxBuffers ];

	/// Non-zero if a video frame is ready
	int						m_nVideoIdx;

	/// The depth format to use
	int						m_nDepthFmt[ 2 ];

	/// Depth buffer
	sqbind::CSqBinary		m_binDepth[ eMaxBuffers ];

	/// Non-zero if a depth frame is ready
	int						m_nDepthIdx;

	/// Log level state
	int						m_nLogLevel[ 2 ];

	/// LED state
	int						m_nLEDState[ 2 ];

	/// Tilt state
	double					m_dTiltState[ 2 ];

};
