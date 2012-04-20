// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_portaudio( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	CPaInput::Register( x_vm );
	CPaOutput::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "pa_input.cpp"
	#include "pa_output.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_portaudio( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

