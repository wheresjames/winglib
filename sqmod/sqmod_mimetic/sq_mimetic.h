
// sq_mimetic.h

#pragma once

class CSqMimetic
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqMimetic )
	_SQBIND_CLASS_CTOR_END( CSqMimetic )

	/// Default constructor
	CSqMimetic();

	/// from mime entity
	CSqMimetic( const mimetic::MimeEntity &r );

	/// Destructor
	virtual ~CSqMimetic();

	/// Releases resources
	void Destroy();

	/// Returns encoded MIME message
	sqbind::stdString Encode();

	/// Parses a MIME message
	int Decode( const sqbind::stdString &s );

	/// returns non-zero if field exists
	int hasField( const sqbind::stdString &s );

	/// Sets from field
	void setFrom( const sqbind::stdString &s );

	/// Gets the from field
	sqbind::stdString CSqMimetic::getFrom();

	/// Sets to field
	void setTo( const sqbind::stdString &s );

	/// Gets the to field
	sqbind::stdString CSqMimetic::getTo();

	/// Sets subject field
	void setSubject( const sqbind::stdString &s );

	/// Gets the subject field
	sqbind::stdString CSqMimetic::getSubject();

	/// Sets body field
	void setBody( const sqbind::stdString &s );

	/// Gets the body field
	sqbind::stdString CSqMimetic::getBody();

	/// Adds / updates / deletes an attachment
	int setAttachment( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pData );

	/// Gets attachment data
	int getAttachment( const sqbind::stdString &sName, sqbind::CSqBinary *pData );

	/// Gets a list of the MIME structure
	sqbind::CSqMulti getStructure();

private:

	/// Mime entity
	mimetic::MimeEntity			m_me;

};
SQBIND_DECLARE_INSTANCE( CSqMimetic, CSqMimetic );

