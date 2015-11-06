// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_tinyxml( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    CSqXml::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "sq_tinyxml.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_tinyxml( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
