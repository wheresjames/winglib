
// sq_smtp.h

#pragma once

class CPoSmtp
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CPoSmtp )
	_SQBIND_CLASS_CTOR_END( CPoSmtp )

	/// Default constructor
	CPoSmtp();

	/// Destructor
	virtual ~CPoSmtp();

	/** \addtogroup CPoSmtp
		@{
	*/

	/// Releases resources
	void Destroy();

	/// Opens the smtp connection
	int Open( const sqbind::stdString &sUrl, int nSecure );

	/// Opens the smtp connection
	int Login( const sqbind::stdString &sType, const sqbind::stdString &sUsername, const sqbind::stdString &sPassword );

	/// Sends the specified message
	int Send( CPoMessage *pMsg );

	/// Returns a string describing the last error that occured
	sqbind::stdString getLastError()
	{	return m_sLastError; }

	/** @} */

private:

	/// SMTP session object
	Poco::Net::SMTPClientSession			*m_pSession;
	
	/// Context for secure connections
	Poco::Net::Context						*m_pContext;

	/// String describing the last error
	sqbind::stdString						m_sLastError;

};

SQBIND_DECLARE_INSTANCE( CPoSmtp, CPoSmtp );

