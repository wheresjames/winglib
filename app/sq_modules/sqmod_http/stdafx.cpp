// stdafx.cpp

#include "stdafx.h"
#include "http_server.h"


// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CHttpServer, "CHttpServer" )
	SQBIND_MEMBER_FUNCTION( CHttpServer, Start )
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CHttpServer );

// Export classes
void SQBIND_Export( sqbind::VM *x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CHttpServer );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
