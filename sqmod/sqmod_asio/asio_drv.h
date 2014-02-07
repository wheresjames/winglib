
#pragma once

class CAsioDrv
{
public:

	enum
	{
		/// The maximum number of channels per driver
		eMaxChannels = 128
	};

	enum
	{
		eDriverMsgNone = 0,

		eDriverMsgReset = 0x0001,

		eDriverMsgResync = 0x0002,

		eDriverMsgLatenciesChanged = 0x0004,

	};

	/// Map shares to channels
	struct SShareInfo
	{
		/// Memory share
		sqbind::CSqFifoShare	share;

		/// Number of active channels
		long					channels;

		/// Channel map for share
		oex::TMem< long >		chmap;
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

	/// Returns the current driver error message
	sqbind::stdString getDriverError();

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
	int OpenChannels( sqbind::CSqMulti *m, int nFlags, int nFmt );

	/// Closes any open channels
	int CloseChannels();

	/// Start the device
	int Start();

	/// Stop the device
	int Stop();

	/// Returns Driver signal flags
	int getDriverSignal() { int n = m_nSignal; m_nSignal &= ~n; return n; }

	/// Returns the number of samples per frame
	long getSamples() { return m_nSamples; }

	/// Returns the number of input channels
	long getInputs() { return m_nInputs; }

	/// Returns the number of input channels
	long getOutputs() { return m_nOutputs; }

	/// Returns the sample type
	long getSampleType() { return m_nSampleType; }

	/// Returns the number of open channels
	long getOpenChannels() { return m_nOpenChannels; }

	/// Returns the number of open shares
	long getOpenShares() { return m_nOpenShares; }

	/// Sets the prefix string
	void setPrefix( const sqbind::stdString &s ) { m_sPrefix = s; }

	/// Returns the current prefix string
	sqbind::stdString getPrefix() { return m_sPrefix; }

	/// Sets the control block id
	void setCbId( oex::oexUINT id ) { m_uCbId = id; }

	/// Gets the control block id
	oex::oexUINT getCbId() { return m_uCbId; }

	/** @} */

private:

	static void _onBufferSwitch( long doubleBufferIndex, ASIOBool directProcess );
	void onBufferSwitch( long doubleBufferIndex, ASIOBool directProcess );

	static void _onSampleRateDidChange( ASIOSampleRate sRate );
	void onSampleRateDidChange( ASIOSampleRate sRate );

	static long _onAsioMessage( long selector, long value, void* message, double* opt );
	long onAsioMessage( long selector, long value, void* message, double* opt );

	static ASIOTime* _onBufferSwitchTimeInfo( ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess );
	ASIOTime* onBufferSwitchTimeInfo( ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess );

	/// Locks the singleton instance
	static oexLock			m_lock;

	/// +++ ASIO doesn't provide a way to pass a user pointer into the callback methods.
	///     For now then, we'll use a single global pointer, limiting us to one running instance.
	static CAsioDrv			*m_pInst;

private:

	/// Maps open channel numbers to objects
	int CreateChannelMap( sqbind::CSqMulti *m );

private:

	/// String describing the last error
	sqbind::stdString					m_sLastError;

	/// ASIO Driver class instance
	AsioDriverList						m_adl;

	/// Driver object
	IASIO								*m_pDriver;

	/// The numbers of samples per frame
	long								m_nSamples;

	/// Number of input channels
	long								m_nInputs;

	/// Number of output channels
	long								m_nOutputs;

	/// The sample type
	long								m_nSampleType;

	/// The number of channels that are open
	long								m_nOpenChannels;

	/// The number of shares that are open
	long								m_nOpenShares;

	/// Share prefix
	sqbind::stdString					m_sPrefix;

	/// Control block id
	oex::oexUINT						m_uCbId;

	/// Driver id
	SQInteger							m_nId;

	/// Driver info
	ASIODriverInfo						m_adi;

	/// ASIO callbacks
	ASIOCallbacks 						m_cb;

	/// ASIO driver buffer info
	oex::TMem< ASIOBufferInfo >			m_bi;

	/// Binary data share
	oex::TMem< SShareInfo >				m_share;

	/// Channel map
	oex::TMem< long >					m_chmap;

	/// Interleave buffer
	sqbind::CSqBinary					m_buf;

	///  Non zero if driver wants quit
	int									m_nSignal;

};
SQBIND_DECLARE_INSTANCE( CAsioDrv, CAsioDrv );

