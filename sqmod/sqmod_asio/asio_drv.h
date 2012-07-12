
#pragma once

class CAsioDrv
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CAsioDrv )
	_SQBIND_CLASS_CTOR_END( CAsioDrv )

	/// Default constructor
	CAsioDrv();

	/// Destructor
	virtual ~CAsioDrv() { Destroy(); }

	/// Copy
	CAsioDrv( const CAsioDrv &r ) { }
	CAsioDrv& operator = ( const CAsioDrv &r ) { return *this; }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CAsioDrv
		@{
	*/

	/// Destroy connection
	void Destroy();
	
	/// Returns the last error string
	sqbind::stdString getLastErrorStr() { return m_sLastError; }

	/// Sets the last error string
	void setLastErrorStr( const sqbind::stdString &s ) { m_sLastError = s; }

	/// Returns a list of available devices
	sqbind::CSqMulti getDevices();

	/** @} */


private:

	/// String describing the last error
	sqbind::stdString	m_sLastError;

	/// Asio driver class
//	AsioDrivers			m_asio;

};

