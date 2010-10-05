
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

	/// Saves the private key to a file
	int SavePrivateKey( const sqbind::stdString &sPrivate );

	/// Saves the public key to a file
	int SavePublicKey( const sqbind::stdString &sPublic );

	/// Load private key from a file
	int LoadPrivateKey( const sqbind::stdString &sFile, const sqbind::stdString &sName );

	/// Load public key
	int LoadPublicKey( const sqbind::stdString &sFile, const sqbind::stdString &sName );

	/// Returns a pointer to the private key
	EVP_PKEY* getPrivateKey() { return m_pkey; }

	/// Returns a pointer to the public key
	EVP_PKEY* getPublicKey() { return m_pkey; }

	/// Returns the key name
	sqbind::stdString getName() { return m_sName; }

	/// Sets the key name
	void setName( const sqbind::stdString &sName )
	{	m_sName = sName; }

private:

	/// Key name
	sqbind::stdString	m_sName;

	/// Private key object
	EVP_PKEY 			*m_pkey;

	/// Public key object
//	NETSCAPE_SPKI 		*m_spki;

};

SQBIND_DECLARE_INSTANCE( COsslKey, COsslKey );

