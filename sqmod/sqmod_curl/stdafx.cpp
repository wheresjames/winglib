// stdafx.cpp

#include "stdafx.h"

// Need to initialize / cleanup openssl
#include <openssl/ssl.h>
#include <openssl/conf.h>
//#include <openssl/engine.h>
#include <openssl/err.h>

// Export classes
static void SQBIND_Export_curl( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Initialize SSL
	SSL_library_init();

	// Initialize curl
	curl_global_init( CURL_GLOBAL_ALL );
		
	CSqCurl::Register( x_vm );
}

static void SQBIND_module_cleanup()
{
	// Cleanup curl
	curl_global_cleanup();

	// SSL cleanup sequence
	ERR_remove_state( 0 );
//	ENGINE_cleanup();
	CONF_modules_unload( 1 );
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}

// Cleanup
#define SQBIND_Exit SQBIND_module_cleanup();

#if defined( SQBIND_STATIC )
	#include "sq_curl.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_curl( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
