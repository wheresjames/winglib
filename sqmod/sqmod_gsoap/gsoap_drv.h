
#pragma once

class CGSoapDrv
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CGSoapDrv )
	_SQBIND_CLASS_CTOR_END( CGSoapDrv )

	/// Default constructor
	CGSoapDrv();

	/// Destructor
	virtual ~CGSoapDrv() { Destroy(); }

	/// Copy
	CGSoapDrv( const CGSoapDrv &r ) { }
	CGSoapDrv& operator = ( const CGSoapDrv &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CGSoapDrv
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Initializes
	void Init();

	/// Returns the last error string
	sqbind::stdString getLastErrorStr() { return m_sLastError; }

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s ) { m_sLastError = s; }

	/** @} */

private:

	/// String describing the last error
	sqbind::stdString					m_sLastError;


};
SQBIND_DECLARE_INSTANCE( CGSoapDrv, CGSoapDrv );

