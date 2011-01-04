// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqSsh2, CSqSsh2 )

	SQBIND_MEMBER_FUNCTION( CSqSsh2, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, Connect )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, setBlockingMode )
	
	SQBIND_MEMBER_FUNCTION( CSqSsh2, setUserInfo )	
	SQBIND_MEMBER_FUNCTION( CSqSsh2, logerr )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getLastError )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getLastErrorCode )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getErrorDesc )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getFingerprint )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, getAuthorizationMethods )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, setUserInfo )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, authUsernamePassword )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, authPublicKey )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, authPublicKeyBin )
	
	SQBIND_MEMBER_FUNCTION( CSqSsh2, CloseAllChannels )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, OpenChannel )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, CloseChannel )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, ChannelSetEnv )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, ChannelRead )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, ChannelWrite )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, MsgFwd )
	SQBIND_MEMBER_FUNCTION( CSqSsh2, OpenChannelDirectTcpip )
	
	SQBIND_MEMBER_FUNCTION( CSqSsh2, print_debug_information )

//	SQBIND_MEMBER_FUNCTION( CSqSsh2,  )
//	SQBIND_MEMBER_FUNCTION( CSqSsh2,  )
//	SQBIND_MEMBER_FUNCTION( CSqSsh2,  )
	

SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_ssh2( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqSsh2 );
}

#if defined( SQBIND_STATIC )
	#include "sq_ssh2.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_ssh2( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
