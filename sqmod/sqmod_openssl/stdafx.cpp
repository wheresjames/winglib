// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqOpenSSL, CSqOpenSSL )

	SQBIND_MEMBER_FUNCTION( CSqOpenSSL, Destroy )

	SQBIND_STATIC_FUNCTION( CSqOpenSSL, Sign )
	SQBIND_STATIC_FUNCTION( CSqOpenSSL, Verify )
	
SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( COsslKey, COsslKey )

	SQBIND_MEMBER_FUNCTION( COsslKey, Destroy )
	
	SQBIND_MEMBER_FUNCTION( COsslKey, CreateRsa )
	SQBIND_MEMBER_FUNCTION( COsslKey, GenPublicKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, SetChallenge )
	SQBIND_MEMBER_FUNCTION( COsslKey, SaveKeys )
	SQBIND_MEMBER_FUNCTION( COsslKey, LoadPrivateKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, LoadPublicKey )

SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_openssl( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqOpenSSL );
    SQBIND_EXPORT( x_vm, COsslKey );
}

#if defined( SQBIND_STATIC )
	#include "sq_openssl.cpp"
	#include "sq_ossl_key.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_openssl( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
