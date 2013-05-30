// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqOpenSSL, CSqOpenSSL )

	SQBIND_MEMBER_FUNCTION( CSqOpenSSL, Destroy )

SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( COsslKey, COsslKey )

	SQBIND_MEMBER_FUNCTION( COsslKey, Destroy )

	SQBIND_MEMBER_FUNCTION( COsslKey, CreateRsa )
	SQBIND_MEMBER_FUNCTION( COsslKey, GenPublicKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, SetChallenge )
	SQBIND_MEMBER_FUNCTION( COsslKey, SaveKeys )
	SQBIND_MEMBER_FUNCTION( COsslKey, SavePrivateKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, SavePublicKey )	
	SQBIND_MEMBER_FUNCTION( COsslKey, LoadPrivateKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, LoadPublicKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, setPrivateKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, getPrivateKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, setPublicKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, getPublicKey )
	SQBIND_MEMBER_FUNCTION( COsslKey, setPrivateKeyRaw )
	SQBIND_MEMBER_FUNCTION( COsslKey, getPrivateKeyRaw )
	SQBIND_MEMBER_FUNCTION( COsslKey, setPublicKeyRaw )
	SQBIND_MEMBER_FUNCTION( COsslKey, getPublicKeyRaw )
	SQBIND_MEMBER_FUNCTION( COsslKey, Sign )
	SQBIND_MEMBER_FUNCTION( COsslKey, SignBin )
	SQBIND_MEMBER_FUNCTION( COsslKey, Verify )
	SQBIND_MEMBER_FUNCTION( COsslKey, VerifyBin )
	SQBIND_MEMBER_FUNCTION( COsslKey, getPassword )
	SQBIND_MEMBER_FUNCTION( COsslKey, setPassword )

SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( COsslCert, COsslCert )

	SQBIND_MEMBER_FUNCTION( COsslCert, Destroy )
	SQBIND_MEMBER_FUNCTION( COsslCert, Create )
	SQBIND_MEMBER_FUNCTION( COsslCert, Sign )
	SQBIND_MEMBER_FUNCTION( COsslCert, SaveFile )

	SQBIND_MEMBER_FUNCTION( COsslCert, SetNameField )
	SQBIND_MEMBER_FUNCTION( COsslCert, SetExtension )

SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( CSqSSLPortFactory, CSqSSLPortFactory )

	SQBIND_MEMBER_FUNCTION( CSqSSLPortFactory, getFactory )
	SQBIND_MEMBER_FUNCTION( CSqSSLPortFactory, Initialize )
	SQBIND_MEMBER_FUNCTION( CSqSSLPortFactory, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqSSLPortFactory, LoadCerts )
	SQBIND_MEMBER_FUNCTION( CSqSSLPortFactory, getLastError )

SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_openssl( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Initialize SSL
	SSL_library_init();
	
//	OPENSSL_config( 0 );
//	OpenSSL_add_all_digests();
//	ERR_load_crypto_strings();

    SQBIND_EXPORT( x_vm, CSqOpenSSL );
    SQBIND_EXPORT( x_vm, COsslKey );
    SQBIND_EXPORT( x_vm, COsslCert );
    SQBIND_EXPORT( x_vm, CSqSSLPortFactory );
}

static void SQBIND_module_cleanup()
{
	// SSL cleanup sequence
	ERR_remove_state( 0 );
#ifndef OPENSSL_NO_ENGINE
	ENGINE_cleanup();
#endif
	CONF_modules_unload( 1 );
	
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
	ERR_remove_thread_state(NULL);
	EVP_cleanup();
}

// Cleanup
#define SQBIND_Exit SQBIND_module_cleanup();

#if defined( SQBIND_STATIC )
	#include "sq_openssl.cpp"
	#include "sq_ossl_key.cpp"
	#include "sq_ossl_cert.cpp"
	#include "sq_ssl_port_factory.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_openssl( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
