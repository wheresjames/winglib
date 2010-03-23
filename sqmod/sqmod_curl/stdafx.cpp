// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqCurl, CSqCurl )
	SQBIND_MEMBER_FUNCTION( CSqCurl, GetLastError )
	SQBIND_MEMBER_FUNCTION( CSqCurl, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqCurl, GetUrl )
	SQBIND_MEMBER_FUNCTION( CSqCurl, PostUrl )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_curl( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqCurl );
}

#if defined( SQBIND_STATIC )
	#include "sq_curl.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_curl( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
