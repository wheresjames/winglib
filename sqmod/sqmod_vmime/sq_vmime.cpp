
// sq_vmime.cpp

#include "stdafx.h"

#if defined( OEX_WINDOWS )
#	include "vmime/platforms/windows/windowsHandler.hpp"
#else
#	include "vmime/platforms/posix/posixHandler.hpp"
#endif

CSqVMime::CSqVMime()
{
#if defined( OEX_WINDOWS )
	vmime::platform::setHandler<vmime::platforms::windows::windowsHandler>();
#else
	vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();
#endif

	m_session = vmime::create < vmime::net::session >();

}

CSqVMime::~CSqVMime()
{
	Destroy();
}

void CSqVMime::Destroy()
{				 
//	m_f.detach();
//	m_st.detach();
//	m_session.detach();
}

int CSqVMime::Open( const sqbind::stdString &sUrl )
{
	if ( !m_session )
		return 0;

	vmime::utility::url url( sUrl.c_str() );

	// Authentication
	vmime::ref <interactiveAuthenticator> ia = vmime::create <interactiveAuthenticator>();
	if ( !ia )
		return 0;

	ia->setUsername( getUsername().c_str() );
	ia->setPassword( getPassword().c_str() );

	try
	{
		// Attempt to open the store
		m_st = m_session->getStore( url, ia );
		if ( !m_st )
			return 0;

		m_st->connect();

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); return 0; }

	try
	{
		m_f = m_st->getDefaultFolder();
		if ( !m_f )
			return 0;

		m_f->open( vmime::net::folder::MODE_READ_WRITE );

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); return 0; }

	return 1;

/*
	vmime::ref <vmime::net::connectionInfos> ci = st->getConnectionInfos();
	if ( !ci )
		return 0;

//	oexSHOW( ci->getHost().c_str() );
//	oexSHOW( ci->getPort() );
//	oexSHOW( st->isSecuredConnection() );
*/
	return 1;
}

int CSqVMime::Close( int expunge )
{
	if ( !m_f )
		return 0;

	try
	{
		m_f->close( expunge );

		return 1;

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}


int CSqVMime::getMessageCount()
{
	if ( !m_f )
		return 0;

	try
	{
		return m_f->getMessageCount();

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

int CSqVMime::deleteMessage( int n )
{
	if ( !m_f )
		return 0;

	try
	{
		m_f->deleteMessage( n );

		return 1;

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

int CSqVMime::deleteMessages( int from, int to )
{
	if ( !m_f )
		return 0;

	try
	{
		m_f->deleteMessages( from, to );

		return 1;

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}


CVmMsg CSqVMime::getMessage( int n )
{
	if ( !m_f )
		return CVmMsg();

	try
	{
		// Get message info
		vmime::ref< vmime::net::message > m = m_f->getMessage( n );

		m_f->fetchMessage( m, vmime::net::folder::FETCH_FLAGS 
							  | vmime::net::folder::FETCH_STRUCTURE
							  | vmime::net::folder::FETCH_FULL_HEADER
							  | vmime::net::folder::FETCH_ENVELOPE
							  | vmime::net::folder::FETCH_SIZE
							  | vmime::net::folder::FETCH_CONTENT_INFO 
					   );

		return m;

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return CVmMsg();
}

int CSqVMime::addMessage( CVmMsg *pMsg )
{
	if ( !m_f )
		return 0;

	try
	{
		m_f->addMessage( pMsg->Obj() );

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

static const vmime::string getFolderPathString(vmime::ref <vmime::net::folder> f)
{
	const vmime::string n = f->getName().getBuffer();
	if ( !n.empty() )
	{	vmime::ref <vmime::net::folder> p = f->getParent();
		return getFolderPathString( p ) + n + oexT( "/" );
	} // end if
	return oexT( "/" );
}

static int mapFolders(sqbind::CSqMulti *m, vmime::ref< vmime::net::folder > folder, const int level = 0)
{
	if ( !m )
		return 0;

	std::vector < vmime::ref < vmime::net::folder> > 
		subFolders = folder->getFolders( false );

	unsigned int subs = subFolders.size();
	if ( !subs )
		return 0;

	int total = subs;
	for ( unsigned int i = 0; i < subs; i++ )
	{	sqbind::CSqMulti &r = (*m)[ subFolders[ i ]->getName().getBuffer().c_str() ];
		total += mapFolders( &r, subFolders[ i ], level + 1 );
	} // end for

	return total;
}

int CSqVMime::getFolders( sqbind::CSqMulti *m )
{
	if ( !m || !m_st )
		return 0;

	try
	{
		vmime::ref< vmime::net::folder > root = m_st->getRootFolder();
		return mapFolders( m, root );

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

int CSqVMime::setFolder( const sqbind::stdString &sFolder )
{
	if ( !m_st )
		return 0;

	try
	{
		oex::CStr s( sFolder.c_str(), sFolder.length() );
		vmime::ref <vmime::net::folder> nf = m_st->getRootFolder();
		
		// Drill down the the desired folder
		while ( s.Length() )
		{
			s.LTrim( oexT( "\\/" ) );

			oex::CStr f = s.Parse( oexT( "\\/" ) );
			if ( f.Length() )
				nf = nf->getFolder( std::string( f.Ptr(), f.Length() ) );

			else if ( s.Length() )
			{	nf = nf->getFolder( std::string( s.Ptr(), s.Length() ) );
				s.Destroy();
			} // end else

		} // end while

		// Open the folder
		nf->open( vmime::net::folder::MODE_READ_WRITE );

		// Lose old folder
		if ( m_f )
			m_f->close( true );

		// Switch to new one
		m_f = nf;

		return 1;

	} // end try
	catch ( vmime::exception e ) { m_sLastError = e.what(); }

	return 0;
}

int CSqVMime::Send( const sqbind::stdString &sUrl, CVmMsg *pMsg )
{
	if ( !pMsg )
		return 0;

	try
	{
		vmime::utility::url url( sUrl.c_str() );

		// New session for sending this message
		vmime::ref< vmime::net::session > session = vmime::create < vmime::net::session >();

		// Authentication
		vmime::ref<interactiveAuthenticator> ia = vmime::create <interactiveAuthenticator>();
		if ( !ia )
			return 0;

		ia->setUsername( getUsername().c_str() );
		ia->setPassword( getPassword().c_str() );

		// Get transport
		vmime::ref <vmime::net::transport> tr = session->getTransport( url, ia );
		if ( !tr )
			return 0;

		// Connec to server
		tr->connect();

		// Send the message
		tr->send( pMsg->Obj() );

		// Disconnect
		tr->disconnect();

	} // end try
	catch ( vmime::exception e ) 
	{
		m_sLastError = e.what(); 
		return 0; 
	} // end catch

	return 1;
}

