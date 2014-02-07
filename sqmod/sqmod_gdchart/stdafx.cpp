// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_gdchart( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	CGdcChart::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "gdc_chart.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_gdchart( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

