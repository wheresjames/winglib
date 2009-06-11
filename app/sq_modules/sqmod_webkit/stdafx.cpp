// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqWebkit, CSqWebkit )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqWebkit );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
