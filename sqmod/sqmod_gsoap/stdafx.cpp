// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_gsoap( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Register class instances
	CGSoapDrv::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "gsoap_drv.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_gsoap( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
