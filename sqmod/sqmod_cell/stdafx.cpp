// stdafx.cpp

#include "stdafx.h"
#include "cell_connection.h"

// Export classes
static void SQBIND_Export_cell( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	CCellConnection::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "cell_backplane.cpp"
	#include "cell_connection.cpp"
	#include "cell_rack.cpp"
	#include "cell_services.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_cell( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
