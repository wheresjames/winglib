// pa_input.h

class CPaInput
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CPaInput )
	_SQBIND_CLASS_CTOR_END( CPaInput )

	/// Default constructor
	CPaInput();

	/// Destructor
	virtual ~CPaInput();

	/** \addtogroup CPaInput
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
	
	/// Starts audio input device
	int Start();

	/// Stops audio input device
	int Stop();

	/// Returns the initialize error
	int getInitError() { return (int)m_errInit; }

	/// Returns the initialize error
	int getLastError() { return (int)m_errLast; }

	/// Returns the default input device
	int getDefaultInputDevice();

	/// Returns the number of available devices
	int getDeviceCount();

	/// Queries information about the specified device
	int getDeviceInfo( int nDev, sqbind::CSqMulti *pInf );

	/// Reads data from the device
	int Read( sqbind::CSqBinary *data, int frames );

	/// Returns number of bytes currently waiting in the buffer
	int getBufferedBytes() { return m_buf.GetMaxRead(); }

	/// Returns the number of bytes in a frame
	int getFrameBytes() { return m_nFrameBytes; }

	/// Returns the number of bytes required for one frame in a particular format
	int getFormatBytes( int nFmt );

	/// Returns the frame block size
	int getFrameBlockSize() { return m_nFrameBlockSize; }

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

	/// Number of frames per block
	int						m_nFrameBlockSize;

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
