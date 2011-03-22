// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqFreenect, CSqFreenect )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, Init )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getNumDevices )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDeviceList )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, OpenDevice )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, CloseDevice )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, Update )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, StartRGB )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, StopRGB )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, StartDepth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, StopDepth )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, setLogLevel )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getErrorLog )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, ClearErrorLog )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, setLED )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, setTilt )
//	SQBIND_MEMBER_FUNCTION( CSqFreenect,  )
//	SQBIND_MEMBER_FUNCTION( CSqFreenect,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CSqFreenect );

// Export classes
static void SQBIND_Export_freenect( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqFreenect );
}

#if defined( SQBIND_STATIC )
	#include "sq_freenect.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_freenect( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

