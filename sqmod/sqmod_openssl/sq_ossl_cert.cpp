// sq_ossl_cert.cpp

#include "stdafx.h"

COsslCert::COsslCert()
{_STT();

	m_pX509 = oexNULL;
}

COsslCert::~COsslCert()
{_STT();
	Destroy();
}

void COsslCert::Destroy()
{_STT();

	if ( m_pX509 )
		X509_free( m_pX509 );

	m_pX509 = oexNULL;
}

sqbind::SQINT COsslCert::Create( COsslKey *x_pKey, sqbind::SQINT x_nSerialNumber, sqbind::SQINT x_nSecondsValid )
{_STT();

	// Lose old cert
	Destroy();

	if ( !x_pKey || ! x_pKey->getPublicKeyPtr() )
		return 0;

	m_pX509 = X509_new();
	if ( !m_pX509 )
		return 0;

	// Set certificate version
	X509_set_version( m_pX509, 2 );
	
	ASN1_INTEGER_set( X509_get_serialNumber( m_pX509 ), x_nSerialNumber );

	X509_gmtime_adj( X509_get_notBefore( m_pX509 ), 0 );

	X509_gmtime_adj( X509_get_notAfter( m_pX509 ), x_nSecondsValid );

	X509_set_pubkey( m_pX509, x_pKey->getPublicKeyPtr() );

	return 1;
}

sqbind::SQINT COsslCert::Sign( COsslKey *x_pKey )
{_STT();

	if ( !m_pX509 || !x_pKey )
		return 0;

	// Sign the cert
	X509_sign( m_pX509, x_pKey->getPublicKeyPtr(), EVP_sha1() );

	return 1;
}

sqbind::SQINT COsslCert::SaveFile( const sqbind::stdString &sFile )
{_STT();

	if ( !m_pX509 )
		return 0;
	
	FILE *fp = fopen( oexStrToMb( sqbind::std2oex( sFile ) ).Ptr(), "w" );
	if ( !fp )
		return 0;

	PEM_write_X509( fp, m_pX509 );

	fclose( fp );

	return 1;

}


sqbind::SQINT COsslCert::SetNameField( const sqbind::stdString &sField, const sqbind::stdString &sValue )
{_STT();

	if ( !m_pX509 )
		return 0;

	if ( !sField.length() )
		return 0;

	// Get name object
	X509_NAME *pName = X509_get_subject_name( m_pX509 );
	if ( !pName )
		return 0;

	// Set specified field
	X509_NAME_add_entry_by_txt( pName, oexStrToMbPtr( sField.c_str() ), MBSTRING_ASC, (const unsigned char *)oexStrToMbPtr( sValue.c_str() ), -1, -1, 0 );

	// Set issuer name to be the same
	X509_set_subject_name( m_pX509, pName ); 
	X509_set_issuer_name( m_pX509, pName ); 

	return 1;
}

#include <crypto/objects/obj_dat.h>
static const ASN1_OBJECT* COSSLCERT_SnToObj( const char * s )
{	long sz = oexSizeOfArray( nid_objs );
	for( long i = 0; i < sz; i++ )
		if ( !oex::zstr::Compare( s, nid_objs[ i ].sn ) )
			return &nid_objs[ i ];
	return oexNULL;
}

static const ASN1_OBJECT* COSSLCERT_LnToObj( const char * s )
{	long sz = oexSizeOfArray( nid_objs );
	for( long i = 0; i < sz; i++ )
		if ( !oex::zstr::Compare( s, nid_objs[ i ].ln ) )
			return &nid_objs[ i ];
	return oexNULL;
}

sqbind::SQINT COsslCert::SetExtension( const sqbind::stdString &sField, const sqbind::stdString &sValue )
{ _STT();

	if ( !m_pX509 )
		return 0;

	if ( !sField.length() )
		return 0;

	// Find this extension by short name
	const ASN1_OBJECT* pObj = COSSLCERT_SnToObj( oexStrToMb( sqbind::std2oex( sField ) ).Ptr() );
	if ( !pObj )
		return 0;

	// Create extension
	X509_EXTENSION *ex = X509V3_EXT_conf_nid( oexNULL, oexNULL, pObj->nid, (char*)oexStrToMb( sqbind::std2oex( sValue ) ).Ptr() );
	if ( !ex )
		return 0;

	// Add to cert
	X509_add_ext( m_pX509, ex, -1 );

	X509_EXTENSION_free( ex );

	return 1;
}



