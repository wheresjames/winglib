// pa_output.h

class CPaOutput
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CPaOutput )
	_SQBIND_CLASS_CTOR_END( CPaOutput )

	/// Default constructor
	CPaOutput();

	/// Destructor
	virtual ~CPaOutput();

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

	/// Returns number of bytes currently waiting in the buffer
	int getBufferedBytes() { return m_buf.GetMaxRead(); }

	/// Returns the number of bytes in a frame
	int getFrameBytes() { return m_nFrameBytes; }

	/// Returns the number of bytes required for one frame in a particular format
	int getFormatBytes( int nFmt );

	/** @} */

protected:

	/// Static callback
	static int _PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags, void *userData );

	/// Callback
	int PaStreamCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo  *timeInfo, PaStreamCallbackFlags  statusFlags );

private:

	/// Non-zero if blocking mode is enabled
	int						m_bBlocking;

	/// Number of bytes in a frame
	int						m_nFrameBytes;

	/// Initialize result
	PaError					m_errInit;	

	/// Last error code
	PaError					m_errLast;	

	/// Output device info
	const PaDeviceInfo		*m_pdi;

	/// Stream
	PaStream				*m_stream;

	/// Thread lock for non-blocking output
//	oexLock					m_lock;

	/// Ring buffer
//	sqbind::CSqBinary		m_buf;

	oex::CCircBuf			m_buf;

};
