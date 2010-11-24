// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqQuickfix, CSqQuickfix )

	SQBIND_MEMBER_FUNCTION( CSqQuickfix, Init )
	SQBIND_MEMBER_FUNCTION( CSqQuickfix, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqQuickfix, getLastError )
	
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CSqQuickfix );

// Export classes
static void SQBIND_Export_quickfix( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqQuickfix );
}

#if defined( SQBIND_STATIC )
	#include "sq_quickfix.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_quickfix( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

