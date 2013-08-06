
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
		
		virtual void SignalStreamRegistered(BaseStream *pStream)
		{
			oexEcho( ">>> CRtmpServer::CRtmpServerApp::SignalStreamRegistered()" );
			return BaseClientApplication::SignalStreamRegistered( pStream );
		}

		virtual bool PullExternalStream(Variant streamConfig)
		{
			oexEcho( ">>> CRtmpServer::CRtmpServerApp::PullExternalStream()" );
			return BaseClientApplication::PullExternalStream( streamConfig );
		}

		virtual bool PushLocalStream(Variant streamConfig)
		{
			oexEcho( ">>> CRtmpServer::CRtmpServerApp::PushLocalStream()" );
			return BaseClientApplication::PushLocalStream( streamConfig );
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
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::CRTMPAppProtocolHandler()" );
		}
		virtual ~CRTMPAppProtocolHandler()
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::~CRTMPAppProtocolHandler()" );
		}

//		bool ProcessInvokeConnect(BaseRTMPProtocol *pFrom, Variant &request);
		virtual bool OutboundConnectionEstablished(OutboundRTMPProtocol *pFrom)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::OutboundConnectionEstablished()" );

			return BaseRTMPAppProtocolHandler::OutboundConnectionEstablished( pFrom );
		}

		virtual bool ProcessInvokeCreateStream(BaseRTMPProtocol *pFrom, Variant &request)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::ProcessInvokeCreateStream()" );

			return BaseRTMPAppProtocolHandler::ProcessInvokeCreateStream( pFrom, request );
		}

		
//		+++ set breakpoint in the play function and see where it goes :)
		
		virtual bool PullExternalStream(URI uri, Variant streamConfig)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::PullExternalStream()" );
			return BaseRTMPAppProtocolHandler::PullExternalStream( uri, streamConfig );
		}

		virtual bool PushLocalStream(Variant streamConfig)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::PushLocalStream()" );
			return BaseRTMPAppProtocolHandler::PushLocalStream( streamConfig );
		}

//		virtual bool PullExternalStreams()
//		virtual bool PullExternalStream(Variant streamConfig);
//		virtual bool PushLocalStream(Variant streamConfig);

		virtual bool AuthenticateInbound(BaseRTMPProtocol *pFrom, Variant &request, Variant &authState)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::AuthenticateInbound()" );
			return BaseRTMPAppProtocolHandler::AuthenticateInbound( pFrom, request, authState );
		}

		virtual bool InboundMessageAvailable(BaseRTMPProtocol *pFrom, Header &header, IOBuffer &inputBuffer)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::InboundMessageAvailable1()" );
			return BaseRTMPAppProtocolHandler::InboundMessageAvailable( pFrom, header, inputBuffer );
		}

		virtual bool InboundMessageAvailable(BaseRTMPProtocol *pFrom, Variant &request)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::InboundMessageAvailable2()" );
			
			string json;
			request.SerializeToJSON(json);
			oexEcho( json.c_str() );
			
			return BaseRTMPAppProtocolHandler::InboundMessageAvailable( pFrom, request );
		}

		virtual bool ProcessInvokePlay(BaseRTMPProtocol *pFrom, Variant &request)
		{
			oexEcho( ">>> CRtmpServer::CRTMPAppProtocolHandler::ProcessInvokePlay()" );
			return BaseRTMPAppProtocolHandler::ProcessInvokePlay( pFrom, request );
		}

	};
	
//	class CLiveStream : public InNetLiveFLVStream
	class CLiveStream : public InboundLiveFLVProtocol
	{
		public:
		
//			CLiveStream() : InNetLiveFLVStream( 0, 0, "livestream" )
			CLiveStream()
			{
				oexEcho( ">>> CRtmpServer::CLiveStream::CLiveStream()" );
			}

			~CLiveStream()
			{
				oexEcho( ">>> CRtmpServer::CLiveStream::~CLiveStream()" );
			}
/*
			virtual bool FeedData(uint8_t *pData, uint32_t dataLength,
								  uint32_t processedLength, uint32_t totalLength,
								  double absoluteTimestamp, bool isAudio)
			{
				oexEcho( "CRtmpServer::CLiveStream::FeedData()" );
				return InNetLiveFLVStream::FeedData( pData, dataLength, processedLength,
													 totalLength, absoluteTimestamp, isAudio );
			}
*/
		private:
		
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

	/// Call continuously to run the socket maintenance
	int CRtmpServer::Run();

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
	
	/// Live stream
	CLiveStream					*m_pLs;

	/// Accept TCP connections
	TCPAcceptor 				*m_pTcpAcceptor;

	/// < 0 if io initialization failed
	int							m_nIoInit;

	/// < 0 if protocol initialization failed
	int							m_nProtoInit;
};

