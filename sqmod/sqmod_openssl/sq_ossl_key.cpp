
// sq_ossl_key.cpp

#include "stdafx.h"

// From spkigen.c
//
/* While I have a 
 * X509_set_pubkey() and X509_REQ_set_pubkey(), SPKI_set_pubkey() does
 * not currently exist so here is a version of it.
 * The next SSLeay release will probably have
 * X509_set_pubkey(),
 * X509_REQ_set_pubkey() and
 * NETSCAPE_SPKI_set_pubkey()
 * as macros calling the same function */
static int SPKI_set_pubkey(NETSCAPE_SPKI *x, EVP_PKEY *pkey)
{
	int ok=0;
	X509_PUBKEY *pk;
	X509_ALGOR *a;
	ASN1_OBJECT *o;
	unsigned char *s,*p;
	int i;

	if (x == NULL) return(0);

	if ((pk=X509_PUBKEY_new()) == NULL) goto err;
	a=pk->algor;

	/* set the algorithm id */
	if ((o=OBJ_nid2obj(pkey->type)) == NULL) goto err;
	ASN1_OBJECT_free(a->algorithm);
	a->algorithm=o;

	/* Set the parameter list */
	if ((a->parameter == NULL) || (a->parameter->type != V_ASN1_NULL))
		{
		ASN1_TYPE_free(a->parameter);
		a->parameter=ASN1_TYPE_new();
		a->parameter->type=V_ASN1_NULL;
		}
	i=i2d_PublicKey(pkey,NULL);
	if ((s=(unsigned char *)malloc(i+1)) == NULL) goto err;
	p=s;
	i2d_PublicKey(pkey,&p);
	if (!ASN1_BIT_STRING_set(pk->public_key,s,i)) goto err;
	free(s);

	X509_PUBKEY_free(x->spkac->pubkey);
	x->spkac->pubkey=pk;
	pk=NULL;
	ok=1;
err:
	if (pk != NULL) X509_PUBKEY_free(pk);
	return(ok);
}


COsslKey::COsslKey()
{_STT();

	m_pkey = oexNULL;
	m_spki = oexNULL;
}

COsslKey::~COsslKey()
{_STT();
	Destroy();
}

void COsslKey::Destroy()
{_STT();

	if ( m_pkey )
		EVP_PKEY_free( m_pkey );

	if ( m_spki )		
		NETSCAPE_SPKI_free( m_spki );
		
	m_pkey = oexNULL;
	m_spki = oexNULL;
}

int COsslKey::CreateRsa( const sqbind::stdString &sKey, int nSize )
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
	
	if ( m_spki )
		NETSCAPE_SPKI_free( m_spki );
	
	/* lets make the spki and set the public key and challenge */
	m_spki = NETSCAPE_SPKI_new();
	if ( !m_spki )
	{	oexERROR( 0, oexT( "NETSCAPE_SPKI_new() failed" ) );
		return 0;
	} // end if

	if ( !SPKI_set_pubkey( m_spki, m_pkey ) )
	{	oexERROR( 0, oexT( "SPKI_set_pubkey() failed" ) );
		NETSCAPE_SPKI_free( m_spki );
		m_spki = oexNULL;
		return 0;
	} // end if
	
	return 1;
	
}
	
int COsslKey::SetChallenge( const sqbind::stdString &sChallenge )
{_STT();
	
	oex::CStr8 mbChallenge = oexStrToMb( oex::CStr( sChallenge.c_str(), sChallenge.length() ) );
	if ( !ASN1_STRING_set( (ASN1_STRING *)m_spki->spkac->challenge, mbChallenge.Ptr(), mbChallenge.Length() ) )
	{	oexERROR( 0, oexT( "ASN1_STRING_set() failed" ) );
		Destroy();
		return 0;
	} // end if
	
	if ( !NETSCAPE_SPKI_sign( m_spki, m_pkey, EVP_md5() ) )
	{	oexERROR( 0, oexT( "NETSCAPE_SPKI_sign() failed" ) );
		Destroy();
		return 0;
	} // end if

	return 1;

}

int COsslKey::SaveKeys( const sqbind::stdString &sName, const sqbind::stdString &sPrivate, const sqbind::stdString &sPublic )
{_STT();

	// Public key?
	if ( m_spki )
	{
		oex::CStr8 name = oexStrToMb( oex::CStr( sPublic.c_str(), sPublic.length() ) );
		FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
		if ( !fp )
			oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( ".pki' ) failed" ) ) );
		else
		{
			PEM_ASN1_write( (int (*)(void*, unsigned char**))i2d_NETSCAPE_SPKI, 
							oexStrToMb( oex::CStr( sName.c_str(), sName.length() ) ).Ptr(), 
							fp, m_spki, NULL, NULL, 0, NULL, NULL );	

			fclose( fp );
			
		} // end else
		
	} // end if

	// Private key?
	if ( m_pkey )
	{
		oex::CStr8 name = oexStrToMb( oex::CStr( sPrivate.c_str(), sPrivate.length() ) );
		FILE *fp = fopen( ( oex::CStr8() << name ).Ptr(), "w" );
		if ( !fp )
			oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( ".key' ) failed" ) ) );
		else
		{

			PEM_write_RSAPrivateKey( fp, m_pkey->pkey.rsa, NULL, NULL, 0, NULL, NULL );

			fclose( fp );
			
		} // end else
		
	} // end if

	return 1;
}

int COsslKey::LoadPrivateKey( const sqbind::stdString &sFile, const sqbind::stdString &sName )
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

int COsslKey::LoadPublicKey( const sqbind::stdString &sFile, const sqbind::stdString &sName )
{_STT();

	Destroy();

	oex::CStr8 name = oexStrToMb( oex::CStr( sFile.c_str(), sFile.length() ) );
	
	FILE *fp = fopen( name.Ptr(), "r" );
	if ( !fp )
	{	oexERROR( oexGetLastError(), oexMks( oexT( "fopen( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		Destroy();
		return 0;
	} // end if

	if ( !PEM_ASN1_read( (void* (*)(void**, const unsigned char**, long int))d2i_NETSCAPE_SPKI, 
				 		 oexStrToMb( oex::CStr( sName.c_str(), sName.length() ) ).Ptr(), 
						 fp, (void**)&m_spki, NULL, NULL ) )
	{	oexERROR( oexGetLastError(), oexMks( oexT( "PEM_ASN1_read( '" ), oexMbToStr( name ), oexT( "' ) failed" ) ) );
		Destroy();
		return 0;
	} // end if


/*
	// Read file
	X509 *x509 = PEM_read_X509( fp, NULL, NULL, NULL );
	if ( !x509 )
	{	oexERROR( 0, oexT( "PEM_read_X509() failed" ) );
		fclose( fp );
		Destroy();
		return 0;
	} // end if
	
	fclose( fp );

	// Find public key
	m_spki = X509_get_pubkey( x509 );
	x509 = oexNULL;
*/

	// Assign key	
	if ( !m_spki )
	{	oexERROR( 0, oexT( "509_get_pubkey() failed" ) );
		Destroy();
		return 0;
	} // end if

	return 1;	
}



