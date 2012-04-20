// pa_output.h

class CPaOutput
{
public:

	enum 
	{
		/// Maximum number of timestamps to hold
		eMaxTimestamps = 1024
	};

	/// Relates timestamps to bytes
	struct STsInfo
	{
		/// Number of frames
		oex::oexINT			nFrames;
	
		/// Timestamp
		oex::oexINT64		ts;
	};


public:

	_SQBIND_CLASS_CTOR_BEGIN( CPaOutput )
	_SQBIND_CLASS_CTOR_END( CPaOutput )

	/// Default constructor
	CPaOutput();

	/// Destructor
	virtual ~CPaOutput();

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CPaOutput
		@{
	*/

	/// Releases resources
	void Destroy();

	/// Initializes output device information
	int Init();

	/// Opens the specified output stream
	int Open( int bBlocking, int nDev, int nChannels, int nFormat, double dLatency, double dSRate, int fpb );

	/// Returns non-zero if a device is open
	int isOpen() { return ( oexNULL != m_stream ) ? 1 : 0; }
	
	/// Starts audio output device
	int Start();

	/// Stops audio output device
	int Stop();

	/// Returns the initialize error
	int getInitError() { return (int)m_errInit; }

	/// Returns the initialize error
	int getLastError() { return (int)m_errLast; }

	/// Returns the default output device
	int getDefaultOutputDevice();

	/// Returns the number of available devices
	int getDeviceCount();

	/// Queries information about the specified device
	int getDeviceInfo( int nDev, sqbind::CSqMulti *pInf );

	/// Writes data to the device
	int Write( sqbind::CSqBinary *data, int frames );

	/// Writes data to the device
	int WriteTs( sqbind::CSqBinary *data, int frames, SQInteger ts );
	
	/// Calculates the current timestamp
	SQInteger getTs();
	
	/// Returns number of bytes currently waiting in the buffer
	int getBufferedBytes() { return m_buf.GetMaxRead(); }

	/// Returns the number of bytes in a frame
	int getFrameBytes() { return m_nFrameBytes; }

	/// Returns the number of bytes required for one frame in a particular format
	int getFormatBytes( int nFmt );

	/// Returns non-zero if glitch detection is enabled
	int getGlitchDetection() { return m_bGlitchDetection; }

	/// Enable / disable glitch detection
	void setGlitchDetection( int b ) { m_bGlitchDetection = b; m_tsGlitch = 0; }

	/// Returns the current glitch size
	SQInteger getGlitchSize() { return m_tsGlitch; }
	
	/// Returns the last sample timestamp added to the buffer
	SQInteger getLastTs() { return m_tsLast; }

	/** @} */

protected:

	/// Static callback
	static int _PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags, void *userData );

	/// Callback
	int PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags );

private:

	/// Non-zero if blocking mode is enabled
	int							m_bBlocking;

	/// Number of bytes in a frame
	int							m_nFrameBytes;
	
	/// Size of a audio block
	int							m_nBlockSize;

	/// Initialize result
	PaError						m_errInit;	

	/// Last error code
	PaError						m_errLast;	

	/// Output device info
	const PaDeviceInfo			*m_pdi;

	/// Stream
	PaStream					*m_stream;

	/// Ring buffer
	oex::CCircBuf				m_buf;
	
	/// Current timestamp write
	int							m_iWTs;
	
	/// Current timestamp read
	int							m_iRTs;
	
	/// Timestamp bytes
	int							m_nTsBytes;
	
	/// Timestamp relation
	STsInfo						m_ts[ eMaxTimestamps ];
	
	/// Glitch detection
	int							m_bGlitchDetection;
	
	/// Glitch correction
	oex::oexINT64				m_tsGlitch;

	/// Last timestamp
	oex::oexINT64				m_tsLast;
	
};
