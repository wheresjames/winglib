// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CCellConnection, CCellConnection )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Connect )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Destroy )
	SQBIND_MEMBER_FUNCTION( CCellConnection, IsConnected )
	SQBIND_MEMBER_FUNCTION( CCellConnection, IsResponding )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetIp )
	SQBIND_MEMBER_FUNCTION( CCellConnection, LoadTags )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetLastError )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetBackplaneData )
	SQBIND_MEMBER_FUNCTION( CCellConnection, ReadTag )
	SQBIND_MEMBER_FUNCTION( CCellConnection, tags )
	SQBIND_MEMBER_FUNCTION( CCellConnection, tmpl )
	SQBIND_MEMBER_FUNCTION( CCellConnection, VerifyTemplate )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetTypeName )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetTypeSize )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetTypeFromName )
	SQBIND_MEMBER_FUNCTION( CCellConnection, SetCacheLimit )
	SQBIND_MEMBER_FUNCTION( CCellConnection, GetCacheLimit )
	SQBIND_MEMBER_FUNCTION( CCellConnection, DumpCache )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CCellConnection );

// Export classes
static void SQBIND_Export_cell( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CCellConnection );
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
