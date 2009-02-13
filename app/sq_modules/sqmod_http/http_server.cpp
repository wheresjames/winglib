// http_server.cpp

#pragma once

#include "stdafx.h"

oex::oexINT CHttpServerImpl::OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										    oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	oexLM();

	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !pServer )
		return -1;

	return 0;
}
