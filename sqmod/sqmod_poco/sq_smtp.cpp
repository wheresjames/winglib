
// sq_smtp.cpp

#include "stdafx.h"

#if defined( oexUNICODE )
static sqbind::stdString StrToWStr( const std::string& s )
{	sqbind::stdString temp( s.length(), L' ' );
	std::copy( s.begin(), s.end(), temp.begin() );
	return temp;
}
static std::string WStrToStr( const sqbind::stdString& s )
{	std::string temp( s.length(), ' ' );
	std::copy( s.begin(), s.end(), temp.begin() );
	return temp;
}
#else
#	define StrToWStr( s )	s.c_str()
#	define WStrToStr( s )	s.c_str()
#endif

CPoSmtp::CPoSmtp()
{_STT();
	m_pSession = oexNULL;
	m_pContext = oexNULL;
}

CPoSmtp::~CPoSmtp()
{_STT();
	Destroy();
}

void CPoSmtp::Destroy()
{_STT();

	try
	{	
		// Drop any session
		if ( m_pSession )
			m_pSession->close();
		
	} // end try
	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
	} // end catch
	
	try
	{	
		// Drop any session
		if ( m_pSession )
		{
			// Secure?
//			if ( m_pContext )
//				OexAllocDelete< Poco::Net::SecureSMTPClientSession >
//					( (Poco::Net::SecureSMTPClientSession*)m_pSession );
//			else
//				OexAllocDelete< Poco::Net::SMTPClientSession >( m_pSession );
		} // end if
		
	} // end try
	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
	} // end catch

	try
	{	
		// Drop any session
//		if ( m_pContext )
//			OexAllocDelete< Poco::Net::Context >( m_pContext );
		
	} // end try
	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
	} // end catch

	m_pContext = oexNULL;
	m_pSession = oexNULL;
}

int CPoSmtp::Open( const sqbind::stdString &sUrl, int nSecure )
{_STT();
	Destroy();

	try
	{	
		if ( nSecure )
		{

			Poco::Net::SecureSMTPClientSession *pSscs = OexAllocConstruct< Poco::Net::SecureSMTPClientSession >( WStrToStr( sUrl ) );
			if ( !pSscs )
			{	m_sLastError = oexT( "Out of memory" );
				return 0;
			} // end if		

			m_pContext = OexAllocConstruct< Poco::Net::Context >( Poco::Net::Context::CLIENT_USE, oexT( "" ), oexT( "" ), oexT( "" ), Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH" );
			if ( !m_pContext )
			{	m_sLastError = oexT( "Out of memory" );
				Destroy();
				return 0;
			} // end if		

			m_pSession = pSscs;

		} // end if

		else
		{
			m_pSession = OexAllocConstruct< Poco::Net::SMTPClientSession >( WStrToStr( sUrl ) );
			if ( !m_pSession )
			{	m_sLastError = oexT( "Out of memory" );
				return 0;
			} // end if		

		} // end else
		
	} // end try
	
	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
		return 0;
	} // end catch
	
	return 1;
}

int CPoSmtp::Login( const sqbind::stdString &sType, const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
{_STT();
	if ( !m_pSession )
	{	m_sLastError = oexT( "Invalid object" );
		return 0;
	} // end if
	
	try
	{	
		Poco::Net::SMTPClientSession::LoginMethod lm = Poco::Net::SMTPClientSession::AUTH_NONE;
		if ( sType == oexT( "CRAMSHA1" ) )
			lm = Poco::Net::SMTPClientSession::AUTH_CRAM_SHA1;	
		else if ( sType == oexT( "CRAMMD5" ) )
			lm = Poco::Net::SMTPClientSession::AUTH_CRAM_MD5;	
		else if ( sType == oexT( "LOGIN" ) )
			lm = Poco::Net::SMTPClientSession::AUTH_LOGIN;

		// TLS?
		if ( m_pContext )
		{
			m_pSession->login( oexT( "SMTP" ) );

			if ( !( (Poco::Net::SecureSMTPClientSession*)m_pSession)->startTLS( m_pContext ) )
			{	m_sLastError = oexT( "Failed to start TLS" );
				return 0;
			} // end if

		} // end if

		m_pSession->login( lm, WStrToStr( sUsername ), WStrToStr( sPassword ) );

	} // end try
	
	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
		return 0;
	} // end catch
	
	return 1;
}

int CPoSmtp::Send( CPoMessage *pMsg )
{_STT();
	if ( !m_pSession || !pMsg )
	{	m_sLastError = oexT( "Invalid object" );
		return 0;
	} // end if

	try
	{
		m_pSession->sendMessage( *pMsg->Ptr() );

	} // end try

	catch ( Poco::Exception& exc )
	{	m_sLastError = StrToWStr( exc.displayText() );
		return 0;
	} // end catch

	return 1;
}
