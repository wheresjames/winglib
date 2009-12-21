
// sq_smtp.h

#pragma once

class CPoSmtp
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CPoSmtp )
	_SQBIND_CLASS_CTOR_END( CPoSmtp )

	/// Default constructor
	CPoSmtp();

	/// Destructor
	virtual ~CPoSmtp();

	/// Releases resources
	void Destroy();


private:

};

SQBIND_DECLARE_INSTANCE( CPoSmtp, CPoSmtp );

