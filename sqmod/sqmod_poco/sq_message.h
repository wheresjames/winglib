
// sq_smtp.h

#pragma once

class CPoMessage
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CPoMessage )
	_SQBIND_CLASS_CTOR_END( CPoMessage )

	/// Default constructor
	CPoMessage();

	/// Destructor
	virtual ~CPoMessage();

	/// Releases resources
	void Destroy();

#define DECLARE_MSG_PROPERTY( p )					\
	void set##p( const sqbind::stdString &s );		\
	sqbind::stdString get##p();

	DECLARE_MSG_PROPERTY( Sender )
	DECLARE_MSG_PROPERTY( Subject )
	DECLARE_MSG_PROPERTY( Content )

	/// Add recipient
	void addRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Add carbon copy recipient
	void addCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Add blind carbon copy recipient
	void addBCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Returns a pointer to the message object
	const Poco::Net::MailMessage* Ptr() { return m_pMsg; }
	
private:

	/// Message object
	Poco::Net::MailMessage				*m_pMsg;
};

SQBIND_DECLARE_INSTANCE( CPoMessage, CPoMessage );

