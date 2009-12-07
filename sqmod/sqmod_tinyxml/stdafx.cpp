// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqXml, CSqXml )
	SQBIND_STATIC_FUNCTION( CSqXml, Encode )
	SQBIND_STATIC_FUNCTION( CSqXml, Decode )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_tinyxml( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqXml );
}

#if defined( SQBIND_STATIC )
	#include "sq_tinyxml.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_tinyxml( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
