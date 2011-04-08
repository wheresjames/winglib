// sq_ossl_cert.h

#pragma once

class COsslCert
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( COsslCert )
	_SQBIND_CLASS_CTOR_END( COsslCert )

	/// Default constructor
	COsslCert();

	/// Destructor
	virtual ~COsslCert();

	/** \addtogroup COsslCert
		@{
	*/

	/// Releases resources
	void Destroy();

	/// Creates a new certificate
	sqbind::SQINT Create( COsslKey *x_pKey, sqbind::SQINT x_nSerialNumber, sqbind::SQINT x_nSecondsValid );

	/// Sets the name field
	sqbind::SQINT SetNameField( const sqbind::stdString &sField, const sqbind::stdString &sValue );

	/// Sets extension value
	sqbind::SQINT SetExtension( const sqbind::stdString &sField, const sqbind::stdString &sValue );

	/// Signs the certificate
	sqbind::SQINT Sign( COsslKey *x_pKey );

	/// Saves the cert to a file
	sqbind::SQINT SaveFile( const sqbind::stdString &sFile );

	/** @} */

private:

	/// The cert
	X509			*m_pX509;

};

SQBIND_DECLARE_INSTANCE( COsslCert, COsslCert );

