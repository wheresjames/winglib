// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqMysql, CSqMysql )
	SQBIND_MEMBER_FUNCTION( CSqMysql, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqMysql, Connect )
	SQBIND_MEMBER_FUNCTION( CSqMysql, Query )
	SQBIND_MEMBER_FUNCTION( CSqMysql, Escape )
	SQBIND_MEMBER_FUNCTION( CSqMysql, MakeInsert )	
	SQBIND_MEMBER_FUNCTION( CSqMysql, MakePairs )	
	SQBIND_MEMBER_FUNCTION( CSqMysql, getNumRows )
	SQBIND_MEMBER_FUNCTION( CSqMysql, getNumFields )
	SQBIND_MEMBER_FUNCTION( CSqMysql, getFieldInfo )	
	SQBIND_MEMBER_FUNCTION( CSqMysql, getRow )
	SQBIND_MEMBER_FUNCTION( CSqMysql, getLastError )
	SQBIND_MEMBER_FUNCTION( CSqMysql, getLastErrorCode )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_mysql( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqMysql );
}

#if defined( SQBIND_STATIC )
	#include "sq_mysql.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_mysql( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
