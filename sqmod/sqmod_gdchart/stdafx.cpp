// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CGdcChart, CGdcChart )

	SQBIND_MEMBER_FUNCTION( CGdcChart, SaveChart )
	SQBIND_MEMBER_FUNCTION( CGdcChart, CreateChart )

//	SQBIND_ENUM( CGdcChart::GDC_LINE, "GDC_LINE" )
//	SQBIND_ENUM( CGdcChart::GDC_LINE, "GDC_AREA" )
//	SQBIND_ENUM( CGdcChart::GDC_LINE, "GDC_BAR" )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CGdcChart );

// Export classes
static void SQBIND_Export_gdchart( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CGdcChart );
}

#if defined( SQBIND_STATIC )
	#include "gdc_chart.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_gdchart( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

