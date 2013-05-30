// stdafx.cpp

#include "stdafx.h"

oexLock _g_crtmp_lock;

// Export classes
static void SQBIND_Export_crtmp( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Register class instances
	CRtmpServer::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "crtmp_server.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_crtmp( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
