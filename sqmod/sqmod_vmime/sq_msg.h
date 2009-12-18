
// sq_msg.h

#pragma once

class CVmMsg
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CVmMsg )
	_SQBIND_CLASS_CTOR_END( CVmMsg )

	/// Default constructor
	CVmMsg();

	/// Copy constructor
	CVmMsg( const CVmMsg &r );

	/// Assignment operator
	CVmMsg& operator = ( const CVmMsg &r );

	/// constructor
	CVmMsg( vmime::ref < vmime::net::message > &r );

	/// constructor
	CVmMsg( vmime::ref < vmime::message > &r );

	/// Destructor
	virtual ~CVmMsg();

	/// Releases resources
	void Destroy();

	/// Returns non-zero if the message is valid
	int isValid() { return !(!m_msg); }

	/// Returns message flags
	int getFlags();

	/// Returns the message header
	sqbind::stdString getHeader();

	/// Checks for the specified fields existence
	int isField( const sqbind::stdString &sName );

	/// Returns the value of the specified field
	sqbind::stdString getField( const sqbind::stdString &sName );

	/// Sets the value of the specified field
	int setField( const sqbind::stdString &sName, const sqbind::stdString &sValue );

	/// Returns the message body
	sqbind::stdString getBody();

	/// Sets the message body
	int setBody( const sqbind::stdString &sBody );

	/// Creates the text representation of the message
	sqbind::stdString Generate();

	/// Returns object reference
	vmime::ref< vmime::message >& Obj() { return m_msg; }

	/// Returns a string describing the message structure
	sqbind::stdString getStructure();

protected: 

	/// Recursive function that helps build structure
	sqbind::stdString _getStructure(vmime::ref <const vmime::net::structure> s, const int level = 0);


private:

	/// Reference to message object
	vmime::ref < vmime::message >			m_msg;
//	vmime::ref < vmime::net::message >		m_msg;

	/// Flags if from net::message
	int										m_flags;

	/// structure if from net::message
	sqbind::stdString						m_sStructure;

	/// Returns the last error
	sqbind::stdString						m_sLastError;
	
};

SQBIND_DECLARE_INSTANCE( CVmMsg, CVmMsg );

