// sq_freenect.h

class CSqFreenect
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqFreenect )
	_SQBIND_CLASS_CTOR_END( CSqFreenect )

	/// Default constructor
	CSqFreenect();

	/// Destructor
	virtual ~CSqFreenect();

	/// Releases resources
	void Destroy();

	/// Initializes output device information
	int Init();

	/// Returns the number of devices
	int getNumDevices();

	/// Opens the specified output stream
	sqbind::CSqMulti getDeviceList();

	/// Opens the specified device
	int OpenDevice( int i );

	/// Closes any open device
	int CloseDevice();	
	
	/// Update device
	int Update();
	
	/// Sets the level of error logging
	void setLogLevel( int level );
	
	/// Returns the error log
	sqbind::stdString getErrorLog() { return m_sErrorLog; }
	
	/// Clears the error log
	void ClearErrorLog() { m_sErrorLog.clear(); }
	
	/// Sets the LED state
	void setLED( int nState );
	
	/// Sets the tilt angle
	void setTilt( double dAngle );
	
	/// Start streaming rgb image
	void StartRGB();
	
	/// Stops streaming rgb image
	void StopRGB();

	/// Starts depth map streaming
	void StartDepth();

	/// Stops depth map streaming
	void StopDepth();
	
private:

	/// Callback proxy
	static void _LogCallback( freenect_context *dev, freenect_loglevel level, const char *msg );
	
	/// Log callback
	static void LogCallback( freenect_loglevel level, const char *msg );
	
	/// Proxy called when there is new depth information
	static void _OnDepth( freenect_device *dev, void *depth, uint32_t timestamp );
	
	/// Called when there is new depth information
	void OnDepth( void *depth, uint32_t timestamp );
	
	/// Proxy called when there is new RGB information
	static void _OnRGB( freenect_device *dev, freenect_pixel *rgb, uint32_t timestamp );

	/// Called when there is new RGB information
	void OnRGB( freenect_pixel *rgb, uint32_t timestamp );

	/// Get raw acceleration?
	sqbind::CSq3dVector getRawAccel();
	
	/// Get some other kind of acceleration?
	sqbind::CSq3dVectord getMksAccel();
	
private:

	/// Initialization status
	int						m_nInit;

	/// Open device index
	int						m_nDev;

	/// freenect context
	freenect_context		*m_pfc;

	/// freenect device
	freenect_device			*m_pfd;
	
	/// Error string
	sqbind::stdString		m_sErrorLog;

};
