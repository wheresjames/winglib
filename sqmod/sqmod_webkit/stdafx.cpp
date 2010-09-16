// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqWebkit, CSqWebkit )
	SQBIND_MEMBER_FUNCTION( CSqWebkit, Destroy )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_webkit( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqWebkit );
}

#if defined( SQBIND_STATIC )
	#include "sq_webkit.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_webkit( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
