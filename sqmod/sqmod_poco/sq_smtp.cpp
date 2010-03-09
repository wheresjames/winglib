
// sq_smtp.cpp

#include "stdafx.h"

CPoSmtp::CPoSmtp()
{_STT();
	m_pSession = oexNULL;
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
	{	m_sLastError = exc.displayText().c_str();
	} // end catch
	
	try
	{	
		// Drop any session
		if ( m_pSession )
			OexAllocDelete< Poco::Net::SMTPClientSession >( m_pSession );
		
	} // end try
	catch ( Poco::Exception& exc )
	{	m_sLastError = exc.displayText().c_str();
	} // end catch

	m_pSession = oexNULL;
}

int CPoSmtp::Open( const sqbind::stdString &sUrl )
{_STT();
	Destroy();

	try
	{	
		m_pSession = OexAllocConstruct< Poco::Net::SMTPClientSession >( sUrl.c_str() );
		if ( !m_pSession )
		{	m_sLastError = oexT( "Out of memory" );
			return 0;
		} // end if		
		
	} // end try
	
	catch ( Poco::Exception& exc )
	{	m_sLastError = exc.displayText().c_str();
		return 0;
	} // end catch
	
	return 1;
}

int CPoSmtp::Login( const sqbind::stdString &sType, const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
{_STT();
	if ( !m_pSession )
	{	m_sLastError = "Invalid object";
		return 0;
	} // end if

	try
	{	
		Poco::Net::SMTPClientSession::LoginMethod lm = Poco::Net::SMTPClientSession::AUTH_NONE;
		if ( sType == "LOGIN" ) 
			lm = Poco::Net::SMTPClientSession::AUTH_LOGIN;
		else if ( sType == "CRAMMD5" ) 
			lm = Poco::Net::SMTPClientSession::AUTH_CRAM_MD5;	
		
		m_pSession->login( lm, sUsername.c_str(), sPassword.c_str() );

	} // end try
	
	catch ( Poco::Exception& exc )
	{	m_sLastError = exc.displayText().c_str();
		return 0;
	} // end catch
	
	return 1;
}

int CPoSmtp::Send( CPoMessage *pMsg )
{_STT();
	if ( !m_pSession || !pMsg )
	{	m_sLastError = "Invalid object";
		return 0;
	} // end if
		
	try
	{	
		m_pSession->sendMessage( *pMsg->Ptr() );

	} // end try
	
	catch ( Poco::Exception& exc )
	{	m_sLastError = exc.displayText().c_str();
		return 0;
	} // end catch
	
	return 1;
}
