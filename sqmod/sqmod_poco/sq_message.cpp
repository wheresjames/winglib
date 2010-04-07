
// sq_message.cpp

#include "stdafx.h"

CPoMessage::CPoMessage()
{_STT();
	m_pMsg = OexAllocConstruct< Poco::Net::MailMessage >();
}

CPoMessage::~CPoMessage()
{_STT();
	if ( m_pMsg )
	{	OexAllocDelete< Poco::Net::MailMessage >( m_pMsg );
		m_pMsg = oexNULL;
	} // end if
}

void CPoMessage::Destroy()
{_STT();
}

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

#define DEFINE_MSG_PROPERTY( p )												\
	void CPoMessage::set##p( const sqbind::stdString &s )						\
	{	if ( m_pMsg )															\
			m_pMsg->set##p( WStrToStr( s ) ); 									\
	}																			\
	sqbind::stdString CPoMessage::get##p()										\
	{	if ( !m_pMsg )															\
			return oexT( "" );													\
		return StrToWStr( m_pMsg->get##p() );									\
	}

DEFINE_MSG_PROPERTY( Sender )
DEFINE_MSG_PROPERTY( Subject )
DEFINE_MSG_PROPERTY( Content )

void CPoMessage::addRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName )
{_STT();
	if ( !m_pMsg )
		return;
	if ( sName.length() )
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::PRIMARY_RECIPIENT,
														WStrToStr( sAddress ), WStrToStr( sName ) ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::PRIMARY_RECIPIENT,
														WStrToStr( sAddress ) ) );
}

void CPoMessage::addCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName )
{_STT();
	if ( !m_pMsg )
		return;
	if ( sName.length() )
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::CC_RECIPIENT,
														WStrToStr( sAddress ), WStrToStr( sName ) ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::CC_RECIPIENT,
														WStrToStr( sAddress ) ) );
}

void CPoMessage::addBCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName )
{_STT();
	if ( !m_pMsg )
		return;
	if ( sName.length() )
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::BCC_RECIPIENT,
														WStrToStr( sAddress ), WStrToStr( sName ) ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::BCC_RECIPIENT,
														WStrToStr( sAddress ) ) );
}

