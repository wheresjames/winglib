
// sq_openssl.cpp

#include "stdafx.h"

CSqOpenSSL::CSqOpenSSL()
{_STT();

//	ERR_load_crypto_strings();

}

CSqOpenSSL::~CSqOpenSSL()
{_STT();
}

void CSqOpenSSL::Destroy()
{_STT();
}

int CSqOpenSSL::Sign( COsslKey *pKey, const sqbind::stdString &sData, sqbind::CSqBinary *sig )
{_STT();

	if ( !pKey || !sig || !pKey->getPrivateKey() )
		return 0;

	EVP_MD_CTX md_ctx;

	// Initialize
	if ( !EVP_SignInit( &md_ctx, EVP_sha1() ) )
	{	oexERROR( 0, oexT( "EVP_SignInit() failed" ) );
		return 0;
	} // end if

	if ( !EVP_SignUpdate( &md_ctx, sData.c_str(), sData.length() ) )
	{	oexERROR( 0, oexT( "EVP_SignUpdate() failed" ) );
		return 0;
	} // end if

	if ( !sig->Allocate( 2 * 1024 ) )
	{	oexERROR( 0, oexT( "EVP_SignUpdate() failed" ) );
		return 0;
	} // end if

	unsigned int sig_len = sig->Size();
	int err = EVP_SignFinal( &md_ctx, (unsigned char*)sig->Ptr(), &sig_len, pKey->getPrivateKey() );
	if ( 1 != err )
	{	oexERROR( err, oexT( "EVP_SignFinal() failed" ) );
		return 0;
	} // end if

	sig->setUsed( sig_len );

	return 1;
}

int CSqOpenSSL::Verify( COsslKey *pKey, const sqbind::stdString &sData, sqbind::CSqBinary *sig )
{_STT();

	if ( !pKey || !sig || !pKey->getPublicKey() )
		return 0;

	EVP_MD_CTX md_ctx;

	if ( !EVP_VerifyInit( &md_ctx, EVP_sha1() ) )
	{	oexERROR( 0, oexT( "EVP_VerifyInit() failed" ) );
		return 0;
	} // end if

	if ( !EVP_VerifyUpdate( &md_ctx, sData.c_str(), sData.length() ) )
	{	oexERROR( 0, oexT( "EVP_VerifyUpdate() failed" ) );
		return 0;
	} // end if

	int err = EVP_VerifyFinal( &md_ctx, (unsigned char*)sig->Ptr(), sig->getUsed(), pKey->getPublicKey() );
	if ( err != 1 )
	{	oexERROR( err, oexT( "EVP_VerifyFinal() failed" ) );
		return 0;
	} // end if

	return 1;
}

