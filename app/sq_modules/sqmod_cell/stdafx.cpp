// stdafx.cpp

#include "stdafx.h"
#include "cell_services.h"


// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CCellConnection, "CCellConnection" )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Connect )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Destroy )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetLastError )
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CCellConnection );

// Export classes
void SQBIND_Export( sqbind::VM *x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CCellConnection );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
