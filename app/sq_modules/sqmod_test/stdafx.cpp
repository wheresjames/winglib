// stdafx.cpp

#include "stdafx.h"
#include "test_class.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CTestClass, CTestClass )
	SQBIND_MEMBER_FUNCTION( CTestClass, Add )
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CTestClass );

// Export classes
static void SQBIND_Export( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CTestClass );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
