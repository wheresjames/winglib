
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
	m_sName = oexT( "" );
}

int COsslKey::CreateRsa( const sqbind::stdString &sName, int nSize )
{_STT();

	// Lose old key
	Destroy();

	// Save key name
	m_sName = sName;

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

int COsslKey::SaveKeys( const sqbind::stdString &sName, const sqbind::stdString &sPrivate, const sqbind::stdString &sPublic )
{_STT();

	// Save key name
	m_sName = sName;

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

int COsslKey::LoadPrivateKey( const sqbind::stdString &sFile, const sqbind::stdString &sName )
{_STT();

	Destroy();

	// Save key name
	m_sName = sName;

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

int COsslKey::LoadPublicKey( const sqbind::stdString &sFile, const sqbind::stdString &sName )
{_STT();

	Destroy();

	// Save key name
	m_sName = sName;

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



