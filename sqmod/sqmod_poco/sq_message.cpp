
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

#define DEFINE_MSG_PROPERTY( p )										\
	void CPoMessage::set##p( const sqbind::stdString &s )				\
	{	if ( m_pMsg )													\
			m_pMsg->set##p( std::string( s.c_str(), s.length() ) ); 	\
	}																	\
	sqbind::stdString CPoMessage::get##p()								\
	{	if ( !m_pMsg )													\
			return oexT( "" );											\
		const std::string &s = m_pMsg->get##p();						\
		return sqbind::stdString( s.c_str(), s.length() ); 				\
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
														sAddress.c_str(), sName.c_str() ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::PRIMARY_RECIPIENT,
														sAddress.c_str() ) );
}

void CPoMessage::addCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName )
{_STT();
	if ( !m_pMsg )
		return;
	if ( sName.length() )
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::CC_RECIPIENT,
														sAddress.c_str(), sName.c_str() ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::CC_RECIPIENT,
														sAddress.c_str() ) );
}

void CPoMessage::addBCCRecipient( const sqbind::stdString &sAddress, const sqbind::stdString &sName )
{_STT();
	if ( !m_pMsg )
		return;
	if ( sName.length() )
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::BCC_RECIPIENT,
														sAddress.c_str(), sName.c_str() ) );
	else
		m_pMsg->addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::BCC_RECIPIENT,
														sAddress.c_str() ) );
}

