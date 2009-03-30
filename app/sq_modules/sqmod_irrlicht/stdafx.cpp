// stdafx.cpp

#include "stdafx.h"
#include "sq_irrlicht.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqIrrlicht, "CSqIrrlicht" )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Init )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Draw )
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CSqIrrlicht );

// Export classes
static void SQBIND_Export( sqbind::VM *x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	oexM();

    SQBIND_EXPORT( x_vm, CSqIrrlicht );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
