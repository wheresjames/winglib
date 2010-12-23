
// sq_ossl_key.cpp

#include "stdafx.h"

COsslKey::COsslKey()
{_STT();

	m_pkey = oexNULL;
}

COsslKey::~COsslKey()
{_STT();
	Destroy();
}

void COsslKey::Destroy()
{_STT();

	if ( m_pkey )
		EVP_PKEY_free( m_pkey );

	m_pkey = oexNULL;
}

int COsslKey::CreateRsa( int nSize )
{_STT();

	// Lose old key
	Destroy();

	// Create a new key
	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Generate RSA key
	RSA *rsa = RSA_generate_key( nSize, RSA_F4, oexNULL, oexNULL );
	if ( !rsa )
	{	oexERROR( 0, oexT( "RSA_generate_key() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	return GenPublicKey();
}

int COsslKey::GenPublicKey()
{_STT();

	return 1;

}

int COsslKey::SetChallenge( const sqbind::stdString &sChallenge )
{_STT();

	return 1;

}

int COsslKey::SaveKeys( const sqbind::stdString &sPrivate, const sqbind::stdString &sPublic )
{_STT();

	// Public key?
	if ( m_pkey )
	{
		oex::CStr8 name = oexStrToMb( oex::CStr( sPublic.c_str(), sPublic.length() ) );
		FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
		if ( !fp )
			oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		else
		{
			PEM_write_RSAPublicKey( fp, m_pkey->pkey.rsa );

			fclose( fp );

		} // end else

	} // end if

	// Private key?
	if ( m_pkey )
	{
		oex::CStr8 name = oexStrToMb( oex::CStr( sPrivate.c_str(), sPrivate.length() ) );
		FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
		if ( !fp )
			oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		else
		{

			PEM_write_RSAPrivateKey( fp, m_pkey->pkey.rsa, NULL, NULL, 0, NULL, NULL );

			fclose( fp );

		} // end else

	} // end if

	return 1;
}

int COsslKey::SavePrivateKey( const sqbind::stdString &sPrivate )
{_STT();

	if ( !m_pkey || !sPrivate.length() )
		return 0;

	oex::CStr8 name = oexStrToMb( oex::CStr( sPrivate.c_str(), sPrivate.length() ) );
	FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
	if ( !fp )
		oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
	else
	{

		PEM_write_RSAPrivateKey( fp, m_pkey->pkey.rsa, NULL, NULL, 0, NULL, NULL );

		fclose( fp );

	} // end else

	return 1;
}

int COsslKey::SavePublicKey( const sqbind::stdString &sPublic )
{_STT();

	if ( !m_pkey || !sPublic.length() )
		return 0;

	oex::CStr8 name = oexStrToMb( oex::CStr( sPublic.c_str(), sPublic.length() ) );
	FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
	if ( !fp )
		oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
	else
	{
		PEM_write_RSAPublicKey( fp, m_pkey->pkey.rsa );

		fclose( fp );

	} // end else

	return 1;
}

int COsslKey::setPrivateKey( sqbind::CSqBinary *pBin )
{_STT();

	Destroy();

	if ( !pBin || !pBin->getUsed() )
		return 0;

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	BIO *pBio = BIO_new_mem_buf( pBin->_Ptr(), pBin->getUsed() );
	if ( !pBio )
	{	oexERROR( 0, oexT( "BIO_new_mem_buf() failed" ) );
		Destroy();
		return 0;
	} // end if

	RSA *rsa = 0;
	if ( !PEM_read_bio_RSAPrivateKey( pBio, &rsa, NULL, NULL ) )
	{	oexERROR( 0, oexT( "PEM_read_bio_RSA_PUBKEY() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	BIO_free( pBio );

	return GenPublicKey();
}

sqbind::CSqBinary COsslKey::getPrivateKey()
{_STT();

	if ( !m_pkey )
		return sqbind::CSqBinary();

	BIO *pBio = BIO_new( BIO_s_mem() );
	if ( !pBio )
	{	oexERROR( 0, oexT( "BIO_new_mem_buf() failed" ) );
		Destroy();
		return sqbind::CSqBinary();
	} // end if

	if ( !PEM_write_bio_RSAPrivateKey( pBio, m_pkey->pkey.rsa, NULL, NULL, 0, NULL, NULL ) )
	{	oexERROR( 0, oexT( "PEM_read_bio_RSA_PUBKEY() failed" ) );
		Destroy();
		return sqbind::CSqBinary();
	} // end if

	BUF_MEM *bm = oexNULL;
	BIO_get_mem_ptr( pBio, &bm );

	sqbind::CSqBinary bin;
	bin.MemCpy( bm->data, bm->length );

	BIO_free( pBio );

	return bin;
}

int COsslKey::setPrivateKeyRaw( sqbind::CSqBinary *pBin )
{_STT();

	Destroy();

	if ( !pBin || !pBin->getUsed() )
		return 0;

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	const unsigned char *p = (const unsigned char*)pBin->Ptr();
	RSA *rsa = d2i_RSAPrivateKey( NULL, &p, pBin->getUsed() );
	if ( !rsa )
	{	oexERROR( 0, oexT( "d2i_RSAPrivateKey() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	return GenPublicKey();
}

sqbind::CSqBinary COsslKey::getPrivateKeyRaw()
{_STT();

	if ( !m_pkey )
		return sqbind::CSqBinary();
		
	// How big is the key?
	int nSize = i2d_RSAPrivateKey( m_pkey->pkey.rsa, NULL );
	if ( 0 >= nSize )
		return sqbind::CSqBinary();

	// Allocate space
	sqbind::CSqBinary bin;
	if ( !bin.Allocate( nSize ) )
		return bin;

	// Get the key data
	unsigned char *p = (unsigned char*)bin._Ptr();
	bin.setUsed( i2d_RSAPrivateKey( m_pkey->pkey.rsa, &p ) );

	return bin;
}

int COsslKey::LoadPrivateKey( const sqbind::stdString &sFile )
{_STT();

	Destroy();

	oex::CStr8 name = oexStrToMb( oex::CStr( sFile.c_str(), sFile.length() ) );

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	FILE *fp = fopen( name.Ptr(), "r" );
	if ( !fp )
	{	oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		Destroy();
		return 0;
	} // end if

	RSA *rsa = PEM_read_RSAPrivateKey( fp, NULL, NULL, NULL );
	if ( !rsa )
	{	oexERROR( 0, oexT( "PEM_read_RSAPrivateKey() failed" ) );
		fclose( fp );
		Destroy();
		return 0;
	} // end if

	fclose( fp );

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	return GenPublicKey();
}

int COsslKey::setPublicKey( sqbind::CSqBinary *pBin )
{_STT();

	Destroy();

	if ( !pBin || !pBin->getUsed() )
		return 0;

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	BIO *pBio = BIO_new_mem_buf( pBin->_Ptr(), pBin->getUsed() );
	if ( !pBio )
	{	oexERROR( 0, oexT( "BIO_new_mem_buf() failed" ) );
		Destroy();
		return 0;
	} // end if

	RSA *rsa = 0;
	if ( !PEM_read_bio_RSAPublicKey( pBio, &rsa, NULL, NULL ) )
	{	oexERROR( 0, oexT( "PEM_read_bio_RSA_PUBKEY() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	BIO_free( pBio );

	return 1;
}

sqbind::CSqBinary COsslKey::getPublicKey()
{_STT();

	if ( !m_pkey )
		return sqbind::CSqBinary();

	BIO *pBio = BIO_new( BIO_s_mem() );
	if ( !pBio )
	{	oexERROR( 0, oexT( "BIO_new_mem_buf() failed" ) );
		Destroy();
		return sqbind::CSqBinary();
	} // end if

	if ( !PEM_write_bio_RSAPublicKey( pBio, m_pkey->pkey.rsa ) )
	{	oexERROR( 0, oexT( "PEM_read_bio_RSA_PUBKEY() failed" ) );
		Destroy();
		return sqbind::CSqBinary();
	} // end if

	BUF_MEM *bm = oexNULL;
	BIO_get_mem_ptr( pBio, &bm );

	sqbind::CSqBinary bin;
	bin.MemCpy( bm->data, bm->length );

	BIO_free( pBio );

	return bin;
}

int COsslKey::setPublicKeyRaw( sqbind::CSqBinary *pBin )
{_STT();

	Destroy();

	if ( !pBin || !pBin->getUsed() )
		return 0;

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	const unsigned char *p = (const unsigned char*)pBin->Ptr();
	RSA *rsa = d2i_RSAPublicKey( NULL, &p, pBin->getUsed() );
	if ( !rsa )
	{	oexERROR( 0, oexT( "d2i_RSAPrivateKey() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	return 1;
}

sqbind::CSqBinary COsslKey::getPublicKeyRaw()
{_STT();

	if ( !m_pkey )
		return sqbind::CSqBinary();
		
	// How big is the key?
	int nSize = i2d_RSAPublicKey( m_pkey->pkey.rsa, NULL );
	if ( 0 >= nSize )
		return sqbind::CSqBinary();

	// Allocate space
	sqbind::CSqBinary bin;
	if ( !bin.Allocate( nSize ) )
		return bin;

	// Get the key data
	unsigned char *p = (unsigned char*)bin._Ptr();
	bin.setUsed( i2d_RSAPublicKey( m_pkey->pkey.rsa, &p ) );

	return bin;
}

int COsslKey::LoadPublicKey( const sqbind::stdString &sFile )
{_STT();

	Destroy();

	m_pkey = EVP_PKEY_new();
	if ( !m_pkey )
	{	oexERROR( 0, oexT( "EVP_PKEY_new() failed" ) );
		Destroy();
		return 0;
	} // end if

	oex::CStr8 name = oexStrToMb( oex::CStr( sFile.c_str(), sFile.length() ) );

	FILE *fp = fopen( name.Ptr(), "r" );
	if ( !fp )
	{	oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		Destroy();
		return 0;
	} // end if

	RSA *rsa = PEM_read_RSAPublicKey( fp, NULL, NULL, NULL );
	if ( !rsa )
	{	oexERROR( 0, oexT( "PEM_read_RSAPublicKey() failed" ) );
		fclose( fp );
		Destroy();
		return 0;
	} // end if

	fclose( fp );

	// Assign key
	if ( !EVP_PKEY_assign_RSA( m_pkey, rsa ) )
	{	oexERROR( 0, oexT( "EVP_PKEY_assign_RSA() failed" ) );
		Destroy();
		return 0;
	} // end if
	rsa = NULL;

	return 1;
}

int COsslKey::Sign( const sqbind::stdString &sData, sqbind::CSqBinary *sig )
{_STT();

	if ( !sig || !getPrivateKeyPtr() )
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
	int err = EVP_SignFinal( &md_ctx, (unsigned char*)sig->Ptr(), &sig_len, getPrivateKeyPtr() );
	if ( 1 != err )
	{	oexERROR( err, oexT( "EVP_SignFinal() failed" ) );
		return 0;
	} // end if

	sig->setUsed( sig_len );

	return 1;
}

int COsslKey::SignBin( sqbind::CSqBinary *pData, sqbind::CSqBinary *sig )
{_STT();

	if ( !pData || !sig || !getPrivateKeyPtr() )
		return 0;

	EVP_MD_CTX md_ctx;

	// Initialize
	if ( !EVP_SignInit( &md_ctx, EVP_sha1() ) )
	{	oexERROR( 0, oexT( "EVP_SignInit() failed" ) );
		return 0;
	} // end if

	if ( !EVP_SignUpdate( &md_ctx, pData->Ptr(), pData->getUsed() ) )
	{	oexERROR( 0, oexT( "EVP_SignUpdate() failed" ) );
		return 0;
	} // end if

	if ( !sig->Allocate( 2 * 1024 ) )
	{	oexERROR( 0, oexT( "EVP_SignUpdate() failed" ) );
		return 0;
	} // end if

	unsigned int sig_len = sig->Size();
	int err = EVP_SignFinal( &md_ctx, (unsigned char*)sig->Ptr(), &sig_len, getPrivateKeyPtr() );
	if ( 1 != err )
	{	oexERROR( err, oexT( "EVP_SignFinal() failed" ) );
		return 0;
	} // end if

	sig->setUsed( sig_len );

	return 1;
}

int COsslKey::Verify( const sqbind::stdString &sData, sqbind::CSqBinary *sig )
{_STT();

	if ( !sig || !getPublicKeyPtr() )
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

	int err = EVP_VerifyFinal( &md_ctx, (unsigned char*)sig->Ptr(), sig->getUsed(), getPublicKeyPtr() );
	if ( err != 1 )
	{	oexERROR( err, oexT( "EVP_VerifyFinal() failed" ) );
		return 0;
	} // end if

	return 1;
}

int COsslKey::VerifyBin( sqbind::CSqBinary *pData, sqbind::CSqBinary *sig )
{_STT();

	if ( !pData || !sig || !getPublicKeyPtr() )
		return 0;

	EVP_MD_CTX md_ctx;

	if ( !EVP_VerifyInit( &md_ctx, EVP_sha1() ) )
	{	oexERROR( 0, oexT( "EVP_VerifyInit() failed" ) );
		return 0;
	} // end if

	if ( !EVP_VerifyUpdate( &md_ctx, pData->Ptr(), pData->getUsed() ) )
	{	oexERROR( 0, oexT( "EVP_VerifyUpdate() failed" ) );
		return 0;
	} // end if

	int err = EVP_VerifyFinal( &md_ctx, (unsigned char*)sig->Ptr(), sig->getUsed(), getPublicKeyPtr() );
	if ( err != 1 )
	{	oexERROR( err, oexT( "EVP_VerifyFinal() failed" ) );
		return 0;
	} // end if

	return 1;
}


