
#pragma once

class CTestClass
{
public:

	enum
	{
		eEnumValue0,

		eEnumValue1,

		eEnumValue2
	};

public:

	/// Register constructors to squirrel
	_SQBIND_CLASS_CTOR_BEGIN( CTestClass )

		// Construct from one integer
		_SQBIND_CLASS_CTOR1( CTestClass, OT_INTEGER ) ( sa.GetInt( 2 ) )

		// Construct from two integers
		_SQBIND_CLASS_CTOR2( CTestClass, OT_INTEGER, OT_INTEGER ) ( sa.GetInt( 2 ), sa.GetInt( 3 ) )

		// Construct from string
		_SQBIND_CLASS_CTOR1( CTestClass, OT_STRING ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ) )

	_SQBIND_CLASS_CTOR_END( CTestClass )

	/// Registers the class
	static void Register( sqbind::VM vm );


public:

	/// Default constructor
	CTestClass() : x( 0 ), y( 0 ) { }

	/// Construct from int
	CTestClass( int _x ) : x( _x ), y( 0 ) { }

	/// Construct from two integers
	CTestClass( int _x, int _y ) : x( _x ), y( _y ) {}

	/// Construct class from string
	CTestClass( const sqbind::stdString &sString ) : m_string( sString ) { x = y = 0; }


public:

	/// Do something simple
	int Add( int x, int y );

	/// Do something simple in a static function
	static int Sub( int x, int y );


private:

	/// String value
	sqbind::stdString		m_string;

	/// Integer values
	int						x, y;

};
