// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqFreenect, CSqFreenect )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, Init )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, isInitialized )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getNumDevices )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, OpenDevice )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, CloseDevice )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getOpenDevice )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, setLogLevel )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getLogLevel )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, setLED )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, setTilt )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, StartVideo )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, StopVideo )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getVideo )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getVideoWidth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getVideoHeight )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getVideoFormat )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getVideoFrameIdx )

	SQBIND_MEMBER_FUNCTION( CSqFreenect, StartDepth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, StopDepth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDepth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDepthWidth )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDepthHeight )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDepthFormat )
	SQBIND_MEMBER_FUNCTION( CSqFreenect, getDepthFrameIdx )

//	SQBIND_MEMBER_FUNCTION( CSqFreenect,  )
//	SQBIND_MEMBER_FUNCTION( CSqFreenect,  )
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

