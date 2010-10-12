
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
	int CreateRsa( int nSize );

	/// Generates a public key from the private key
	int GenPublicKey();

	/// Sets the password challenge on the key
	int SetChallenge( const sqbind::stdString &sChallenge );

	/// Save keys to a file
	int SaveKeys( const sqbind::stdString &sPrivate, const sqbind::stdString &sPublic );

	/// Saves the private key to a file
	int SavePrivateKey( const sqbind::stdString &sPrivate );

	/// Saves the public key to a file
	int SavePublicKey( const sqbind::stdString &sPublic );

	/// Load private key from a file
	int LoadPrivateKey( const sqbind::stdString &sFile );

	/// Load public key
	int LoadPublicKey( const sqbind::stdString &sFile );

	/// Load private key from memory buffer
	int setPrivateKey( sqbind::CSqBinary *pBin );

	/// Loads a public key from a buffer
	int setPublicKey( sqbind::CSqBinary *pBin );

	/// Returns the priate key in a binary buffer
	sqbind::CSqBinary getPrivateKey();

	/// Returns the public key in a binary buffer
	sqbind::CSqBinary getPublicKey();

	/// Sign data
	int Sign( const sqbind::stdString &sData, sqbind::CSqBinary *sig );

	/// Sign data
	int SignBin(sqbind::CSqBinary *pData, sqbind::CSqBinary *sig );

	/// Verify sig
	int Verify( const sqbind::stdString &sData, sqbind::CSqBinary *sig );

	/// Verify sig
	int VerifyBin( sqbind::CSqBinary *pData, sqbind::CSqBinary *sig );

	/// Returns a pointer to the private key
	EVP_PKEY* getPrivateKeyPtr() { return m_pkey; }

	/// Returns a pointer to the public key
	EVP_PKEY* getPublicKeyPtr() { return m_pkey; }

	/// Returns the key password
	sqbind::stdString getPassword() { return sqbind::oex2std( oexMbToStr( m_sPassword ) ); }

	/// Sets the key password
	void setPassword( const sqbind::stdString &s ) { m_sPassword = oexStrToMb( sqbind::std2oex( s ) ); }

	/// Returns a pointer to the password phrase or null if none
	char* getPasswordPtr() { return m_sPassword.Length() ? (char*)m_sPassword.Ptr() : NULL; }

private:

	/// Key object
	EVP_PKEY 			*m_pkey;

	/// Key password
	oex::CStr8			m_sPassword;

};

SQBIND_DECLARE_INSTANCE( COsslKey, COsslKey );

