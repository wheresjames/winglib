
// sq_mimetic.cpp

#include "stdafx.h"

CSqMimetic::CSqMimetic()
{
}

CSqMimetic::~CSqMimetic()
{
	Destroy();
}

void CSqMimetic::Destroy()
{
}

/*
static void printMimeStructure(mimetic::MimeEntity* pMe, int tabcount = 0)
{
	mimetic::Header& h = pMe->header();										// get header object
	for(int c = tabcount; c > 0; --c)										// indent nested entities
		oexPrintf( " - " );													//
	oexPrintf( oexMks( h.contentType().str().c_str(), oexNL ).Ptr() );      // prints Content-Type
	mimetic::MimeEntityList& parts = pMe->body().parts();					// list of sub entities obj
	// cycle on sub entities list and print info of every item
	mimetic::MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
	for(; mbit != meit; ++mbit)
		printMimeStructure(*mbit, 1 + tabcount);
}
*/

int CSqMimetic::Decode( const sqbind::stdString &s )
{
//	printMimeStructure( &m_me );
	std::stringstream ss; ss.write( s.c_str(), s.length() );
	m_me.load( ss );
	return m_me.size();
}

sqbind::stdString CSqMimetic::Encode()
{
	std::stringstream ss; ss << m_me;
	return sqbind::stdString( ss.str().c_str(), ss.str().length() );
}

void CSqMimetic::setFrom( const sqbind::stdString &s )
{
	m_me.header().from( s.c_str() );
}

sqbind::stdString CSqMimetic::getFrom()
{
	return sqbind::stdString( m_me.header().from().str().c_str(), 
						      m_me.header().from().str().length() );
}

void CSqMimetic::setTo( const sqbind::stdString &s )
{
	m_me.header().to( s.c_str() );
}

sqbind::stdString CSqMimetic::getTo()
{
	return sqbind::stdString( m_me.header().to().str().c_str(), 
						      m_me.header().to().str().length() );
}

void CSqMimetic::setSubject( const sqbind::stdString &s )
{
	m_me.header().subject( s.c_str() );
}

sqbind::stdString CSqMimetic::getSubject()
{
	return sqbind::stdString( m_me.header().subject().c_str(), 
						      m_me.header().subject().length() );
}

void CSqMimetic::setBody( const sqbind::stdString &s )
{
	m_me.body().assign( s.c_str() );
}

sqbind::stdString CSqMimetic::getBody()
{	
	std::stringstream ss; ss << m_me.body();
	return sqbind::stdString( ss.str().c_str(), ss.str().length() );
}

