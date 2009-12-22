// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CPoSmtp, CPoSmtp )

	SQBIND_MEMBER_FUNCTION( CPoSmtp, Destroy )
	SQBIND_MEMBER_FUNCTION( CPoSmtp, Open )
	SQBIND_MEMBER_FUNCTION( CPoSmtp, Login )
	SQBIND_MEMBER_FUNCTION( CPoSmtp, Send )
	
	SQBIND_MEMBER_FUNCTION( CPoSmtp, getLastError )

SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( CPoMessage, CPoMessage )

	SQBIND_MEMBER_FUNCTION( CPoMessage, Destroy )

	SQBIND_MEMBER_FUNCTION( CPoMessage, setSender )
	SQBIND_MEMBER_FUNCTION( CPoMessage, getSender )
	SQBIND_MEMBER_FUNCTION( CPoMessage, setSubject )
	SQBIND_MEMBER_FUNCTION( CPoMessage, getSubject )
	SQBIND_MEMBER_FUNCTION( CPoMessage, setContent )
	SQBIND_MEMBER_FUNCTION( CPoMessage, getContent )
	SQBIND_MEMBER_FUNCTION( CPoMessage, addRecipient )
	SQBIND_MEMBER_FUNCTION( CPoMessage, addCCRecipient )
	SQBIND_MEMBER_FUNCTION( CPoMessage, addBCCRecipient )
	
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_poco( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CPoSmtp );
    SQBIND_EXPORT( x_vm, CPoMessage );
}

#if defined( SQBIND_STATIC )
	#include "sq_smtp.cpp"
	#include "sq_message.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_poco( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
