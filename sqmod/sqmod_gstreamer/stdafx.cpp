// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CGsCapture, CGsCapture )

	SQBIND_MEMBER_FUNCTION( CGsCapture, Destroy )
	SQBIND_MEMBER_FUNCTION( CGsCapture, Init )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CGsCapture );

// Export classes
static void SQBIND_Export_gscapture( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CGsCapture );
}

#if defined( SQBIND_STATIC )
	#include "gs_capture.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_gscapture( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
