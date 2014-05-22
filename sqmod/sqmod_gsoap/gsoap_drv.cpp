
#include "stdafx.h"

// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CGSoapDrv, CGSoapDrv )

	SQBIND_MEMBER_FUNCTION( CGSoapDrv, Init )
	SQBIND_MEMBER_FUNCTION( CGSoapDrv, Destroy )
	SQBIND_MEMBER_FUNCTION( CGSoapDrv, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CGSoapDrv, setLastErrorStr )
//	SQBIND_MEMBER_FUNCTION( CGSoapDrv,  )
//	SQBIND_MEMBER_FUNCTION( CGSoapDrv,  )
//	SQBIND_MEMBER_FUNCTION( CGSoapDrv,  )

SQBIND_REGISTER_CLASS_END()

void CGSoapDrv::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CGSoapDrv );
}

CGSoapDrv::CGSoapDrv()
{_STT();
}

void CGSoapDrv::Destroy()
{_STT();
}

void CGSoapDrv::Init()
{
	struct soap soap;
//	SOAP_ENV__Header header;

	soap_init(&soap); 
	
	soap.connect_flags = SO_BROADCAST; 
	
	soap.send_timeout = 10;   
	soap.recv_timeout = 10;  

	soap.ipv4_multicast_ttl = 1;
	
//	soap_default_SOAP_ENV__Header(&soap, &header); 
	soap.header = &header; 
	
	soap_default_wsa5__EndpointReferenceType(&soap, &replyTo); 
	replyTo.Address = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous"; 
	
	header.wsa5__MessageID = "uuid:12345678-1234-1234-123456789abc"; 
	header.wsa5__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery"; 
	header.wsa5__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"; 
	header.wsa5__ReplyTo = &replyTo; 

	struct d__ProbeType dn__Probe;
	soap_default_d__ProbeType(&soap, &dn__Probe);

	struct d__ProbeMatchesType dn__ProbeResponse;

	struct d__ScopesType a;
	soap_default_d__ScopesType(&soap, &a);
	soap_put_d__ScopesType(&soap, &a, NULL, "onvif://www.onvif.org");	
	dn__Probe.Scopes = &a;
	soap_put_d__ProbeType(&soap, &dn__Probe, NULL, "dn:NetworkVideoTransmitter");
	
	if (soap_call___ns2__Probe(&soap, "soap.udp://239.255.255.250:3702", NULL, &dn__Probe,
&dn__ProbeResponse)) {
	   soap_print_fault(&soap, stderr); 
	}
	else
	{ 
		printf("found!\n");
		printf("XAddrs:%s\n", dn__ProbeResponse.ProbeMatch->XAddrs);

}