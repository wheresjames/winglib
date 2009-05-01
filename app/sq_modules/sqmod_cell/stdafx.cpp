// stdafx.cpp

#include "stdafx.h"
#include "cell_services.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CCellConnection, CCellConnection )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Connect )
	SQBIND_MEMBER_FUNCTION( CCellConnection, Destroy )
	SQBIND_MEMBER_FUNCTION( CCellConnection, IsConnected )
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
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CCellConnection );

// Export classes
static void SQBIND_Export( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CCellConnection );
}

// Include squirrel module exported symbols
#include <sqmod_extern.hpp>
