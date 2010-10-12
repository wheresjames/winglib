
// sq_openssl.h

#pragma once

class CSqOpenSSL
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqOpenSSL )
	_SQBIND_CLASS_CTOR_END( CSqOpenSSL )

	/// Default constructor
	CSqOpenSSL();

	/// Destructor
	virtual ~CSqOpenSSL();

	/// Releases resources
	void Destroy();

private:

};

SQBIND_DECLARE_INSTANCE( CSqOpenSSL, CSqOpenSSL );

