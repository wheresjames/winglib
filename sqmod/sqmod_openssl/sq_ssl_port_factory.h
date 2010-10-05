
// sq_ssl_port.h

#pragma once

class CSqSSLPortFactory// : public oex::os::CIpSocket
{
public:
	_SQBIND_CLASS_CTOR_BEGIN( CSqSSLPortFactory )
	_SQBIND_CLASS_CTOR_END( CSqSSLPortFactory )

	/// Default constructor
	CSqSSLPortFactory();

	/// Destructor
	virtual ~CSqSSLPortFactory();

	class CSqSSLPort : public oex::os::CIpSocket
	{
	public:

		/// Default constructor
		CSqSSLPort( SSL_CTX *ctx );

		/// Destructor
		virtual ~CSqSSLPort();

		/// Returns a description of the last error
		sqbind::stdString getLastError() { return m_sLastError; }

	public:

		/// Called when opening a new socket
		virtual oex::oexBOOL OnAttach();

		/// Called before closing a socket
		virtual oex::oexBOOL OnClose();

		/// Override to provide custom read
		virtual int v_recv( int socket, void *buffer, int length, int flags );
		virtual int v_recvfrom( int socket, void *buffer, int length, int flags );

		/// Override to provide custom write
		virtual int v_send( int socket, const void *buffer, int length, int flags );
		virtual int v_sendto(int socket, const void *message, int length, int flags );

		virtual oex::oexCSTR v_get_transport_type() { return oexT( "https" ); }
		virtual oex::oexCSTR v_get_transport_name() { return oexT( "OpenSSL" ); }

	private:

		/// Last error
		sqbind::stdString	m_sLastError;

		/// SSL context
		SSL_CTX 			*m_ctx;

		/// SSL object
		SSL					*m_ssl;

	};

	class CPortFactory : public oex::CFactory
	{
	public:

		/// Constructor
		CPortFactory();

		/// Destructor
		virtual ~CPortFactory();

		/// Initializes the SSL context
		int Initialize();

		/// Releases SSL resources
		void Destroy();

		/// Loads certificates
		int LoadCerts( const sqbind::stdString &sCertChain, const sqbind::stdString &sPrivateKey );

		/// Returns a string describing the last error
		sqbind::stdString getLastError() { return m_sLastError; }

	public:

		/// Returns non-zero if there is valid memory
		virtual oex::oexBOOL IsValid() { return oex::oexTRUE; }

		/// Creates an object instance
		virtual oex::oexPVOID Create()
		{	return (oex::oexPVOID)(oex::os::CIpSocket*)OexAllocConstruct< CSqSSLPort >( m_ctx ); }

		/// Frees an object instance
		virtual void Free( oex::oexPVOID p )
		{	if ( p ) OexAllocDestruct< CSqSSLPort >( (CSqSSLPort*)(oex::os::CIpSocket*)p ); }

	protected:

		/// Initializes the SSL context
		int CreateCtx();

	private:

		/// SSL context
		SSL_CTX 			*m_ctx;

		/// String describing the last error
		sqbind::stdString		m_sLastError;

	};

	/// Returns the port factory
	sqbind::CSqFactory getFactory() { return (oex::CFactory*)&m_cFactory; }

	/// Initializes the SSL context
	int Initialize()
	{	return m_cFactory.Initialize(); }

	/// Releases SSL resources
	void Destroy()
	{	m_cFactory.Destroy(); }

	/// Loads certificates
	int LoadCerts( const sqbind::stdString &sCertChain, const sqbind::stdString &sPrivateKey )
	{	return m_cFactory.LoadCerts( sCertChain, sPrivateKey ); }

	/// Returns a string describing the last error
	sqbind::stdString getLastError() { return m_cFactory.getLastError(); }

private:

	/// Port facotry
	CPortFactory			m_cFactory;

};

SQBIND_DECLARE_INSTANCE( CSqSSLPortFactory, CSqSSLPortFactory );

