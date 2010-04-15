
// sq_ossl_key.h

#pragma once

class COsslKey
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( COsslKey )
	_SQBIND_CLASS_CTOR_END( COsslKey )

	/// Default constructor
	COsslKey();

	/// Destructor
	virtual ~COsslKey();

	/// Releases resources
	void Destroy();
	
	/// Creates the specified key
	int CreateRsa( const sqbind::stdString &sKey, int nSize );

	/// Generates a public key from the private key
	int GenPublicKey();

	/// Sets the password challenge on the key
	int SetChallenge( const sqbind::stdString &sChallenge );

	/// Save keys to a file
	int SaveKeys( const sqbind::stdString &sName, const sqbind::stdString &sPrivate, const sqbind::stdString &sPublic );

	/// Load private key from a file
	int LoadPrivateKey( const sqbind::stdString &sFile, const sqbind::stdString &sName );
	
	/// Load public key
	int LoadPublicKey( const sqbind::stdString &sFile, const sqbind::stdString &sName );
	
	EVP_PKEY* getPrivateKey() { return m_pkey; }
	
	NETSCAPE_SPKI* getPublicKey() { return m_spki; }
	
private:

	/// Private key object
	EVP_PKEY 			*m_pkey;
	
	/// Public key object
	NETSCAPE_SPKI 		*m_spki;

};

SQBIND_DECLARE_INSTANCE( COsslKey, COsslKey );

