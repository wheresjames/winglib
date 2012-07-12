
#include "stdafx.h"

#define MY_TEST_CONSTANT 10

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CTestClass, CTestClass )

	// Class member
	SQBIND_MEMBER_FUNCTION( CTestClass, Add )

	// Static class member
	SQBIND_STATIC_FUNCTION( CTestClass, Sub )

	// Enum
	SQBIND_ENUM( CTestClass::eEnumValue1, eEnumValue1 )

	// Global constant
	SQBIND_GLOBALCONST( MY_TEST_CONSTANT )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CTestClass );


void CTestClass::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CTestClass );
}

int CTestClass::Add( int x, int y )
{_STT();
	return x + y; 
}

// static
int CTestClass::Sub( int x, int y )
{_STT();
	return x - y;
}
