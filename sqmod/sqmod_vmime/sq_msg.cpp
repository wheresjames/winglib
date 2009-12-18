
// sq_msg.cpp

#include "stdafx.h"

CVmMsg::CVmMsg()
{
	m_flags = 0;
	m_msg = vmime::create< vmime::message >();
}

CVmMsg::CVmMsg( vmime::ref < vmime::net::message > &r )
{
	m_flags = 0;

	try
	{
		if ( r )
		{
			m_flags = r->getFlags();
			m_sStructure = _getStructure( r->getStructure() );

			m_msg = vmime::create< vmime::message >();
			if ( m_msg )
			{	std::stringstream ss;
				vmime::utility::outputStreamAdapter out( ss );
				r->extract( out );
				m_msg->parse( ss.str() );
			} // end if

		} // end if

	} catch( vmime::exception e ) { m_sLastError = e.what(); }
}

CVmMsg::CVmMsg( vmime::ref < vmime::message > &r )
{
	m_flags = 0;
	if ( r )
		m_msg = r;
}

CVmMsg::CVmMsg( const CVmMsg &r )
{
	m_flags = r.m_flags;
	m_sStructure = r.m_sStructure;
	if ( r.m_msg )
		m_msg = r.m_msg;
}

CVmMsg& CVmMsg::operator = ( const CVmMsg &r )
{
	m_flags = r.m_flags;
	m_sStructure = r.m_sStructure;
	if ( r.m_msg )
		m_msg = r.m_msg;
	return *this;
}

CVmMsg::~CVmMsg()
{
	Destroy();
}

void CVmMsg::Destroy()
{
	m_flags = 0;
	m_sStructure = oexT( "" );
//	m_msg.detach();
}

int CVmMsg::getFlags()
{	
	return m_flags;
}


sqbind::stdString CVmMsg::getHeader()
{
	if ( !m_msg )
		return oexT( "" );

	try
	{	std::string s = m_msg->getHeader()->generate();
		return sqbind::stdString( s.c_str(), s.length() );
	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return oexT( "" );
}

int CVmMsg::isField( const sqbind::stdString &sName )
{
	if ( !m_msg )
		return 0;

	try
	{	return m_msg->getHeader()->hasField( sName.c_str() ) ? 1 : 0;
	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return 0;
}

sqbind::stdString CVmMsg::getField( const sqbind::stdString &sName )
{
	if ( !m_msg )
		return oexT( "" );

	try
	{	vmime::ref <vmime::headerField> f = m_msg->getHeader()->findField( sName.c_str() );
		if ( !f ) return oexT( "" );
		std::string s = f->generate();
		return sqbind::stdString( s.c_str(), s.length() );
	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return oexT( "" );
}

int CVmMsg::setField( const sqbind::stdString &sName, const sqbind::stdString &sValue )
{
	if ( !m_msg )
		return 0;

	try
	{	vmime::ref <vmime::headerField> f = m_msg->getHeader()->getField( sName.c_str() );
		if ( !f ) return 0;
		std::string s( sValue.c_str(), sValue.length() ); 
		f->setValue( s );
		return 1;
	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return 0;
}

sqbind::stdString CVmMsg::Generate()
{
	if ( !m_msg )
		return oexT( "" );

	try
	{
		std::stringstream ss;
		vmime::utility::outputStreamAdapter out( ss );
		m_msg->generate( out );
		return sqbind::stdString( ss.str().c_str(), ss.str().length() );

	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return oexT( "" );
}

sqbind::stdString CVmMsg::_getStructure(vmime::ref <const vmime::net::structure> s, const int level)
{
	sqbind::stdString sRet;
	for ( int i = 0; i < s->getPartCount(); i++ )
	{
		vmime::ref <const vmime::net::part> part = s->getPartAt(i);

		for ( int j = 0; j < level * 2; j++ )
			sRet += oexT( " " );

		// Add the 
		sRet += oexMks( ( part->getNumber() + 1 ), ". ",
					 part->getType().generate().c_str(),
					 " [", part->getSize(), " byte(s)]",
					 oexNL ).Ptr();
/*
		std::stringstream ss;
		vmime::utility::outputStreamAdapter out( ss );
		m_msg->extractPart( part, out );
		s += sqbind::stdString( ss.str().c_str(), ss.str().length() );
*/

		sRet += _getStructure( part->getStructure(), level + 1 );
	}

	return sRet;
}

sqbind::stdString CVmMsg::getStructure()
{
	return m_sStructure;
}


sqbind::stdString CVmMsg::getBody()
{
	if ( !m_msg )
		return oexT( "" );

	try
	{	vmime::ref< vmime::body > body = m_msg->getBody();
		std::stringstream ss;
		vmime::utility::outputStreamAdapter out( ss );
		body->generate( out );
		return sqbind::stdString( ss.str().c_str(), ss.str().length() );
	} catch( vmime::exception e ) { m_sLastError = e.what(); }
	return oexT( "" );
}

int CVmMsg::setBody( const sqbind::stdString &sBody )
{
	if ( !m_msg )
		return 0;

	try
	{
		vmime::ref< vmime::body > body = m_msg->getBody();
		body->parse( sBody.c_str(), 0, sBody.length() );
		return 1;

	} catch( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

