
#pragma once

// Statically linking modules?
#if defined( SQBIND_STATIC )

#	if defined( SQBIND_STATIC_cell )
#		include "../../sqmod/sqmod_cell/stdafx.cpp"
#	endif

#	if defined( SQBIND_STATIC_curl )
#		include "../../sqmod/sqmod_curl/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_ffmpeg )
#	include "../../sqmod/sqmod_ffmpeg/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_gdchart )
#	include "../../sqmod/sqmod_gdchart/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_irrlicht )
#	include "../../sqmod/sqmod_irrlicht/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_live555 )
#	include "../../sqmod/sqmod_live555/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_tinyxml )
#	include "../../sqmod/sqmod_tinyxml/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_webkit )
#	include "../../sqmod/sqmod_webkit/stdafx.cpp"
#endif

    static oex::oexRESULT SQBIND_Export_Symbols( sqbind::VM x_vm, sqbind::SSqAllocator *x_pAllocator )
	{_STT();

#if defined( SQBIND_STATIC_cell )
		SQBIND_Export_cell( x_vm );
#endif

#if defined( SQBIND_STATIC_curl )
		SQBIND_Export_curl( x_vm );
#endif

#if defined( SQBIND_STATIC_gdchart )
		SQBIND_Export_gdchart( x_vm );
#endif

#if defined( SQBIND_STATIC_ffmpeg )
		SQBIND_Export_ffmpeg( x_vm );
#endif

#if defined( SQBIND_STATIC_irrlicht )
		SQBIND_Export_irrlicht( x_vm );
#endif

#if defined( SQBIND_STATIC_live555 )
		SQBIND_Export_live555( x_vm );
#endif

#if defined( SQBIND_STATIC_tinyxml )
		SQBIND_Export_tinyxml( x_vm );
#endif

#if defined( SQBIND_STATIC_webkit )
		SQBIND_Export_webkit( x_vm );
#endif

		return 0;
	}

#else

#	define SQBIND_Export_Symbols oexNULL

#endif
