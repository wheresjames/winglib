// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqVMime, CSqVMime )
	SQBIND_MEMBER_FUNCTION( CSqVMime, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqVMime, Open )
	SQBIND_MEMBER_FUNCTION( CSqVMime, Close )
	SQBIND_MEMBER_FUNCTION( CSqVMime, getLastError )
	SQBIND_MEMBER_FUNCTION( CSqVMime, getUsername )
	SQBIND_MEMBER_FUNCTION( CSqVMime, setUsername )
	SQBIND_MEMBER_FUNCTION( CSqVMime, getPassword )
	SQBIND_MEMBER_FUNCTION( CSqVMime, setPassword )

	SQBIND_MEMBER_FUNCTION( CSqVMime, getMessageCount )
	SQBIND_MEMBER_FUNCTION( CSqVMime, getMessage )
	SQBIND_MEMBER_FUNCTION( CSqVMime, addMessage )

	SQBIND_MEMBER_FUNCTION( CSqVMime, getFolders )
	SQBIND_MEMBER_FUNCTION( CSqVMime, setFolder )

	SQBIND_MEMBER_FUNCTION( CSqVMime, deleteMessage )
	SQBIND_MEMBER_FUNCTION( CSqVMime, deleteMessages )	

	SQBIND_MEMBER_FUNCTION( CSqVMime, Send )

SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( CVmMsg, CVmMsg )

	SQBIND_MEMBER_FUNCTION( CVmMsg, Destroy )
	SQBIND_MEMBER_FUNCTION( CVmMsg, isValid )

	SQBIND_MEMBER_FUNCTION( CVmMsg, getHeader )
	SQBIND_MEMBER_FUNCTION( CVmMsg, isField )
	SQBIND_MEMBER_FUNCTION( CVmMsg, getField )
	SQBIND_MEMBER_FUNCTION( CVmMsg, setField )
	SQBIND_MEMBER_FUNCTION( CVmMsg, getBody )
	SQBIND_MEMBER_FUNCTION( CVmMsg, setBody )

	SQBIND_MEMBER_FUNCTION( CVmMsg, getStructure )
	SQBIND_MEMBER_FUNCTION( CVmMsg, Generate )

	SQBIND_CONST( vmime::net::message, FLAG_SEEN )
	SQBIND_CONST( vmime::net::message, FLAG_RECENT )
	SQBIND_CONST( vmime::net::message, FLAG_REPLIED )
	SQBIND_CONST( vmime::net::message, FLAG_DELETED )
	SQBIND_CONST( vmime::net::message, FLAG_MARKED )
	SQBIND_CONST( vmime::net::message, FLAG_PASSED )

SQBIND_REGISTER_CLASS_END()


// Export classes
static void SQBIND_Export_vmime( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqVMime );
    SQBIND_EXPORT( x_vm, CVmMsg );
}

#if defined( SQBIND_STATIC )
	#include "sq_vmime.cpp"
	#include "sq_msg.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_vmime( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
