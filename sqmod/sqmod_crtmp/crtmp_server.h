
#pragma once

class CRtmpServer
{
public:

	class CRtmpServerApp : public BaseClientApplication
	{
	public:
//		CRtmpServerApp(Variant &configuration);
//		virtual ~CRtmpServerApp();

		CRtmpServerApp(Variant &configuration)
			: BaseClientApplication(configuration)
		{
//			_pRTMPHandler = new MonitorRTMPProtocolHandler(configuration);
//			RegisterAppProtocolHandler(PT_MONITORRTMP, _pRTMPHandler);
		}

		~CRtmpServerApp()
		{
		}


//	private:
//		MonitorRTMPProtocolHandler *_pRTMPHandler;

	};


	/// Custom rtmp protocol handler
	class CRTMPAppProtocolHandler : public BaseRTMPAppProtocolHandler
	{
	public:
		CRTMPAppProtocolHandler(Variant &configuration)
			: BaseRTMPAppProtocolHandler(configuration)
		{
		}
		virtual ~CRTMPAppProtocolHandler()
		{
		}

//		bool ProcessInvokeConnect(BaseRTMPProtocol *pFrom, Variant &request);
//		virtual bool OutboundConnectionEstablished(OutboundRTMPProtocol *pFrom);
	};

public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CRtmpServer )
	_SQBIND_CLASS_CTOR_END( CRtmpServer )

	/// Default constructor
	CRtmpServer();

	/// Destructor
	virtual ~CRtmpServer() { Destroy(); }

	/// Copy
	CRtmpServer( const CRtmpServer &r ) { }
	CRtmpServer& operator = ( const CRtmpServer &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CRtmpServer
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Initializes the RTMP session
	int Init();

	/// Start the server
	int Start( int x_nPort );

	/// Returns the last error string
	sqbind::stdString getLastErrorStr() { return m_sLastError; }

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s ) { m_sLastError = s; }

	/// Return protocol map
	sqbind::stdString CRtmpServer::getProtocolMap();

	/** @} */

private:

	/// String describing the last error
	sqbind::stdString			m_sLastError;

	/// Conifiguration
	Variant						m_cConfig;

	/// RTMP App level object
	CRtmpServerApp				*m_pSa;

	/// Default protocol factory
	DefaultProtocolFactory 		*m_pPf;

	/// Protocol handler
	CRTMPAppProtocolHandler		*m_pPh;

	/// Accept TCP connections
	TCPAcceptor 				*m_pTcpAcceptor;

	/// < 0 if io initialization failed
	int							m_nIoInit;

	/// < 0 if protocol initialization failed
	int							m_nProtoInit;
};

