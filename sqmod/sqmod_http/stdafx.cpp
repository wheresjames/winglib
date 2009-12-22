// stdafx.cpp

#include "stdafx.h"
#include "http_server.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CHttpServer, CHttpServer )
	SQBIND_MEMBER_FUNCTION( CHttpServer, Start )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetSessionCallback )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetSessionCallbackScript )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetAuthenticationCallback )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetServerCallback )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetLogFile )
	SQBIND_MEMBER_FUNCTION( CHttpServer, EnableRemoteConnections )
	SQBIND_MEMBER_FUNCTION( CHttpServer, EnableSessions )
	SQBIND_MEMBER_FUNCTION( CHttpServer, EnableMultiThreading )
	SQBIND_MEMBER_FUNCTION( CHttpServer, SetSessionTimeout )
	SQBIND_MEMBER_FUNCTION( CHttpServer, GetNumActiveClients )
	SQBIND_MEMBER_FUNCTION( CHttpServer, MapFolder )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CHttpServer );

// Export classes
static void SQBIND_Export_http( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CHttpServer );
}

#if defined( SQBIND_STATIC )
	#include "http_server.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_http( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
