// http_server.cpp

#include "stdafx.h"

oex::oexINT CHttpServerImpl::OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										    oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	oexSHOWL( x_nEvent );
	oexSHOWL( x_nErr );

	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !pServer )
		return -1;

	return 0;
}

oex::oexINT CHttpServerImpl::OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !pServer )
		return -1;

	x_pSession->Content() << oexT( "Hello World!" );

	return 0;
}

