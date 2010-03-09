
// sq_webkit.h

#pragma once

class CSqWebkit
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqWebkit )
	_SQBIND_CLASS_CTOR_END( CSqWebkit )

	/// Default constructor
	CSqWebkit();

	/// Destructor
	~CSqWebkit();

	/// Releases resources
	void Destroy();


private:


};
SQBIND_DECLARE_INSTANCE( CSqWebkit, CSqWebkit );

