// stdafx.cpp

#include "stdafx.h"
#include "sq_irrlicht.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqIrrlicht, CSqIrrlicht )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Init )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, Draw )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, AddSkyDome )

	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetStereo )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetStereo )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetREyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetREyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, SetLEyeKey )
	SQBIND_MEMBER_FUNCTION( CSqIrrlicht, GetLEyeKey )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CSqIrrlicht );

// Export classes
static void SQBIND_Export( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqIrrlicht );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
