
// sq_vmime.h

#pragma once

class CSqVMime
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqVMime )
	_SQBIND_CLASS_CTOR_END( CSqVMime )

	/// Default constructor
	CSqVMime();

	/// Destructor
	virtual ~CSqVMime();

	/// Releases resources
	void Destroy();

private:

	/// Session object
	vmime::ref <vmime::net::session> m_session;

};

SQBIND_DECLARE_INSTANCE( CSqVMime, CSqVMime );

