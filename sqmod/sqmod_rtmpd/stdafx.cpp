// stdafx.cpp

#include "stdafx.h"

#if defined( _DEBUG )
	extern "C" FILE *netstackdump = 0;
	extern "C" FILE *netstackdump_read = 0;
#endif

// Export classes
static void SQBIND_Export_rtmpd( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Register class instances
	CRtmpdSession::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "rtmpd_session.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_rtmpd( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
