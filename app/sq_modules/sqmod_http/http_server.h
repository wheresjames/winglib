// http_server.h

#pragma once

class CHttpServerImpl
{
public:

	virtual int Start( int nPort )
	{
		// Start the server
		if ( !m_server.StartServer( nPort, CHttpServerImpl::OnServerEvent, this ) )
			return 0;

		return 1;
	}

	/// Server callback
	static oex::oexINT OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
								      oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer );

private:

	// The server
	oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > m_server;

};

class CHttpServer
{
public:

	CHttpServer()
	{
		m_pServer = oexNULL;
	}

	~CHttpServer()
	{
		if ( m_pServer )
		{	delete m_pServer;
			m_pServer = oexNULL;
		} // end if
	}

	void Init()
	{	if ( !m_pServer )
			m_pServer = new CHttpServerImpl();
	}

	virtual int Start( int nPort )
	{
		Init();

		return m_pServer->Start( nPort );
	}

private:

	/// Pointer to server object
	CHttpServerImpl		*m_pServer;

};
