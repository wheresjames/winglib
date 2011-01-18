
// sq_message.cpp

#include "stdafx.h"

CPoMessage::CPoMessage()
{_STT();
	m_pMsg = OexAllocConstruct< Poco::Net::MailMessage >();
}

CPoMessage::CPoMessage( const sqbind::stdString &s )
{
	m_pMsg = OexAllocConstruct< Poco::Net::MailMessage >();
	DecodeStr( s );
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
#	define StrToWStr( s )	sqbind::stdString( s.c_str(), s.length() )
#	define WStrToStr( s )	std::string( s.c_str(), s.length() )
#endif
#define BinToStr( b )		std::string( (b).Ptr(), (b).getUsed() )

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
DEFINE_MSG_PROPERTY( ContentType )

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

int CPoMessage::Encode( sqbind::CSqBinary *pBin )
{
	if ( !pBin )
		return 0;

	if ( !m_pMsg )
	{	pBin->setUsed( 0 );
		return 0;
	} // end if

	sqbind::stdOStream out;
	m_pMsg->write( out );

	pBin->setString( out.str() );
	return pBin->getUsed();
}

sqbind::stdString CPoMessage::EncodeStr()
{
	if ( !m_pMsg )
		return sqbind::stdString();

	sqbind::stdOStream out;
	m_pMsg->write( out );

	return out.str();
}

int CPoMessage::Decode( sqbind::CSqBinary *pBin )
{
	if ( !pBin )
		return 0;

	if ( !m_pMsg )
		return 0;

	sqbind::stdIStream in( pBin->getString() );
	m_pMsg->read( in );

	return 1;
}

int CPoMessage::DecodeStr( const sqbind::stdString &s )
{
	if ( !s.length() )
		return 0;

	if ( !m_pMsg )
		return 0;

	sqbind::stdIStream in( s );
	m_pMsg->read( in );

	return 1;
}


int CPoMessage::addPartFile( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sFile )
{
	if ( !m_pMsg || !sType.length() || !sFile.length() )
		return 0;

	m_pMsg->addPart( WStrToStr( sName ),
					 new Poco::Net::FilePartSource( WStrToStr( sFile ), WStrToStr( sType ) ),
					 Poco::Net::MailMessage::CONTENT_INLINE, Poco::Net::MailMessage::ENCODING_QUOTED_PRINTABLE
					 );

	return 1;
}

int CPoMessage::addPartBin( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pBin )
{
	if ( !m_pMsg || !sType.length() || !pBin || !pBin->getUsed() )
		return 0;

	m_pMsg->addPart( WStrToStr( sName ), 
					 new Poco::Net::StringPartSource( BinToStr( *pBin ), WStrToStr( sType ), WStrToStr( sName ) ),
					 Poco::Net::MailMessage::CONTENT_INLINE, Poco::Net::MailMessage::ENCODING_QUOTED_PRINTABLE
					 );

	return pBin->getUsed();
}

int CPoMessage::addPartStr( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sContent )
{
	if ( !m_pMsg || !sType.length() )
		return 0;

	m_pMsg->addPart( WStrToStr( sName ), 
					 new Poco::Net::StringPartSource( WStrToStr( sContent ), WStrToStr( sType ), WStrToStr( sName ) ),
					 Poco::Net::MailMessage::CONTENT_INLINE, Poco::Net::MailMessage::ENCODING_QUOTED_PRINTABLE
					 );

	return sContent.length();
}

int CPoMessage::addAttachmentFile( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sFile )
{
	if ( !m_pMsg || !sType.length() || !sFile.length() )
		return 0;

	m_pMsg->addAttachment( WStrToStr( ( sName.length() ? sName : sFile ) ),
						   new Poco::Net::FilePartSource( WStrToStr( sFile ), WStrToStr( sType ) ) );

	return 1;
}

int CPoMessage::addAttachmentBin( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pBin )
{
	if ( !m_pMsg || !sName.length() || !sType.length() || !pBin || !pBin->getUsed() )
		return 0;

	m_pMsg->addAttachment( WStrToStr( sName ), new Poco::Net::StringPartSource( BinToStr( *pBin ), WStrToStr( sType ), WStrToStr( sName ) ) );

	return pBin->getUsed();
}

int CPoMessage::addAttachmentStr( const sqbind::stdString &sName, const sqbind::stdString &sType, const sqbind::stdString &sContent )
{
	if ( !m_pMsg || !sName.length() || !sType.length() )
		return 0;

	m_pMsg->addAttachment( WStrToStr( sName ), new Poco::Net::StringPartSource( WStrToStr( sContent ), WStrToStr( sType ), WStrToStr( sName ) ) );

	return sContent.length();
}

int CPoMessage::setPartHeader( const sqbind::stdString &sPart, const sqbind::stdString &sName, const sqbind::stdString &sValue )
{
	if ( !m_pMsg || !sPart.length() || !sName.length() )
		return 0;

	m_pMsg->setPartHeader( WStrToStr( sPart ), WStrToStr( sName ), WStrToStr( sValue ) );

	return 1;
}
