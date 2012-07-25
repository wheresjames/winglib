
#pragma once

class CAsioDrv
{
public:
	enum
	{
		eDriverMsgNone = 0,
		
		eDriverMsgReset = 0x0001,
		
		eDriverMsgResync = 0x0002,
		
		eDriverMsgLatenciesChanged = 0x0004,
		
	};

public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CAsioDrv )
	_SQBIND_CLASS_CTOR_END( CAsioDrv )

	/// Default constructor
	CAsioDrv();

	/// Destructor
	virtual ~CAsioDrv() { Destroy(); }

	/// Copy
	CAsioDrv( const CAsioDrv &r ) { }
	CAsioDrv& operator = ( const CAsioDrv &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CAsioDrv
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Returns the last error string
	sqbind::stdString getLastErrorStr() { return m_sLastError; }

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s ) { m_sLastError = s; }

	/// Returns a list of available devices
	sqbind::CSqMulti getDevices();

	/// Returns the driver id
	SQInteger getId() { return m_nId; }

	/// Returns non zero if driver is loaded
	int isDriver() { return ( m_pDriver && 0 <= m_nId ) ? 1 : 0; }

	/// Loads the specified driver
	int LoadDriver( int nId );
	
	/// Get driver info
	sqbind::CSqMulti getInfo();
	
	/// Set sample rate
	void setSampleRate( double d );

	/// Get sample rate
	double getSampleRate();

	/// Open specified channels
	int OpenChannels( sqbind::CSqMulti *m, const sqbind::stdString &sShare, int nFlags, int nFmt );
	
	/// Closes any open channels
	int CloseChannels();
	
	/// Start the device
	int Start();

	/// Stop the device
	int Stop();

	/// Returns Driver signal flags
	int getDriverSignal() { int n = m_nSignal; m_nSignal &= ~n; return n; }
	
	/** @} */


private:

	static void onBufferSwitch(long doubleBufferIndex, ASIOBool directProcess);

	static void onSampleRateDidChange(ASIOSampleRate sRate);

	static long onAsioMessage(long selector, long value, void* message, double* opt);

	static ASIOTime* onBufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);

private:

	/// String describing the last error
	sqbind::stdString				m_sLastError;

	/// ASIO Driver class instance
	AsioDriverList					m_adl;

	/// Driver object
	IASIO							*m_pDriver;

	/// Driver id
	SQInteger						m_nId;

	/// Driver info
	ASIODriverInfo					m_adi;

	/// ASIO driver buffer info
	oex::TMem< ASIOBufferInfo >		m_bi;

	/// ASIO callbacks
	ASIOCallbacks 					m_cb;
	
	/// Binary data share
	sqbind::CSqBinaryShare			m_share;
	
	///  Non zero if driver wants quit
	int								m_nSignal;
};

