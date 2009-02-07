// stdafx.cpp

#include "stdafx.h"
#include "test_class.h"


// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CTestClass )
	SQBIND_MEMBER_FUNCTION( CTestClass, Add )
SQBIND_REGISTER_CLASS_END()


// Export classes
void SQMOD_Export( sqbind::VM *x_vm )
{ 
    SQBIND_EXPORT( x_vm, CTestClass );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
