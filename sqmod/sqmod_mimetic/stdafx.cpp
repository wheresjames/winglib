// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqMimetic, CSqMimetic )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, setFrom )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, getFrom )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, setTo )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, getTo )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, setSubject )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, getSubject )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, setBody )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, getBody )
	SQBIND_MEMBER_FUNCTION( CSqMimetic, Encode )	
	SQBIND_MEMBER_FUNCTION( CSqMimetic, Decode )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_mimetic( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqMimetic );
}

#if defined( SQBIND_STATIC )
	#include "sq_mimetic.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_mimetic( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
