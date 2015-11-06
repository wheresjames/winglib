
#pragma once

class CSqHash
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqHash )
	_SQBIND_CLASS_CTOR_END( CSqHash )

	/// Default constructor
	CSqHash();

	/// Destructor
	virtual ~CSqHash();

	static void Register( sqbind::VM vm );
	
	/** \addtogroup CSqHash
		@{
	*/

	/// Releases resources
	void Destroy();

	sqbind::CSqBinary sha1( sqbind::CSqBinary *bin );
	
	/** @} */

private:

};

DECLARE_INSTANCE_TYPE( CSqHash );

