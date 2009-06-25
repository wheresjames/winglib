
#pragma once

// Statically linking modules?
#if defined( SQBIND_STATIC )

#	if defined( SQBIND_STATIC_cell )
#		include "../sq_modules/sqmod_cell/stdafx.cpp"
#	endif

#	if defined( SQBIND_STATIC_curl )
#		include "../sq_modules/sqmod_curl/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_gdchart )
#	include "../sq_modules/sqmod_gdchart/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_http )
#	include "../sq_modules/sqmod_http/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_irrlicht )
#	include "../sq_modules/sqmod_irrlicht/stdafx.cpp"
#endif

#	if defined( SQBIND_STATIC_webkit )
#	include "../sq_modules/sqmod_webkit/stdafx.cpp"
#endif

    static oex::oexRESULT SQBIND_Export_Symbols( sqbind::VM x_vm, sqbind::SSqAllocator *x_pAllocator )
	{

#if defined( SQBIND_STATIC_cell )
		SQBIND_Export_cell( x_vm );
#endif

#if defined( SQBIND_STATIC_curl )
		SQBIND_Export_curl( x_vm );
#endif

#if defined( SQBIND_STATIC_gdchart )
		SQBIND_Export_gdchart( x_vm );
#endif

#if defined( SQBIND_STATIC_http )
		SQBIND_Export_http( x_vm );
#endif

#if defined( SQBIND_STATIC_irrlicht )
		SQBIND_Export_irrlicht( x_vm );
#endif

#if defined( SQBIND_STATIC_webkit )
		SQBIND_Export_webkit( x_vm );
#endif

		return 0;
	}

#endif

