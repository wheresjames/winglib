// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_freetype2( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	CFtLibrary::Register( x_vm );
	CFtFace::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "sq_face.cpp"
	#include "sq_library.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_freetype2( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
