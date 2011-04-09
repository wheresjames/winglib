
// sq_smtp.h

#pragma once

class CPoMessage
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CPoMessage )
		_SQBIND_CLASS_CTOR1( CPoMessage, OT_STRING ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ) )
	_SQBIND_CLASS_CTOR_END( CPoMessage )

	/// Default constructor
	CPoMessage();

	/// From MIME string
	CPoMessage( const sqbind::stdString &s );

	/// Destructor
	virtual ~CPoMessage();

	/// Returns a pointer to the message object
	const Poco::Net::MailMessage* Ptr() { return m_pMsg; }

	/** \addtogroup CPoMessage
		@{
	*/

	/// Releases resources
	void Destroy();

#define DECLARE_MSG_PROPERTY( p )					\
	void set##p( const sqbind::stdString &s );		\
	sqbind::stdString get##p();

	DECLARE_MSG_PROPERTY( Sender )
	DECLARE_MSG_PROPERTY( Subject )
	DECLARE_MSG_PROPERTY( Content )
	DECLARE_MSG_PROPERTY( ContentType )

	/// Add recipient
	void addRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Add carbon copy recipient
	void addCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Add blind carbon copy recipient
	void addBCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName );
	
	/// Encodes MIME to a buffer
	int Encode( sqbind::CSqBinary *pBin );

	/// Encodes MIME and returns as a string
	sqbind::stdString EncodeStr();

	/// Decodes MIME from a buffer
	int Decode( sqbind::CSqBinary *pBin );

	/// Decodes MIME from a string
	int DecodeStr( const sqbind::stdString &s );

	/// Add part from disk
	int addPartFile( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sFile );

	/// Add part from string
	int addPartStr( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sContent );

	/// Add part from binary buffer
	int addPartBin( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pBin );
	
	/// Attach file from disk
	int addAttachmentFile( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sFile );

	/// Attaches a string
	int addAttachmentStr( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sContent );

	/// Attach binary buffer
	int addAttachmentBin( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pBin );
	
	/// Sets part header values
	int setPartHeader( const sqbind::stdString &sPart, const sqbind::stdString &sName, const sqbind::stdString &sValue );

	/** @} */

private:

	/// Message object
	Poco::Net::MailMessage				*m_pMsg;
};

SQBIND_DECLARE_INSTANCE( CPoMessage, CPoMessage );

