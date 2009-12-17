// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqVMime, CSqVMime )
	SQBIND_MEMBER_FUNCTION( CSqVMime, Destroy )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_vmime( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqVMime );
}

#if defined( SQBIND_STATIC )
	#include "sq_vmime.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_vmime( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
