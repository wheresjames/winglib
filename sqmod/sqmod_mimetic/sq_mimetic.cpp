
// sq_mimetic.cpp

#include "stdafx.h"

CSqMimetic::CSqMimetic()
{_STT();
}

CSqMimetic::CSqMimetic( const mimetic::MimeEntity &r )
	: m_me( r )
{_STT();
}


CSqMimetic::~CSqMimetic()
{_STT();
	Destroy();
}

void CSqMimetic::Destroy()
{_STT();
}


int CSqMimetic::Decode( const sqbind::stdString &s )
{_STT();
//	printMimeStructure( &m_me );
	std::stringstream ss; ss.write( s.c_str(), s.length() );
	m_me.load( ss );
	return m_me.size();
}

sqbind::stdString CSqMimetic::Encode()
{_STT();
	std::stringstream ss; ss << m_me;
	return sqbind::stdString( ss.str().c_str(), ss.str().length() );
}

void CSqMimetic::setFrom( const sqbind::stdString &s )
{_STT();
	m_me.header().from( s.c_str() );
}

sqbind::stdString CSqMimetic::getFrom()
{_STT();
	return sqbind::stdString( m_me.header().from().str().c_str(), 
							  m_me.header().from().str().length() );
}

void CSqMimetic::setTo( const sqbind::stdString &s )
{_STT();
	m_me.header().to( s.c_str() );
}

sqbind::stdString CSqMimetic::getTo()
{_STT();
	return sqbind::stdString( m_me.header().to().str().c_str(), 
							  m_me.header().to().str().length() );
}

void CSqMimetic::setSubject( const sqbind::stdString &s )
{_STT();
	m_me.header().subject( s.c_str() );
}

sqbind::stdString CSqMimetic::getSubject()
{_STT();
	return sqbind::stdString( m_me.header().subject().c_str(), 
							  m_me.header().subject().length() );
}

void CSqMimetic::setBody( const sqbind::stdString &s )
{_STT();
	m_me.body().assign( s.c_str() );
}

sqbind::stdString CSqMimetic::getBody()
{_STT();
	std::stringstream ss; ss << m_me.body();
	return sqbind::stdString( ss.str().c_str(), ss.str().length() );
}

int CSqMimetic::hasField( const sqbind::stdString &s )
{_STT();
	return m_me.header().hasField( s.c_str() ) ? 1 : 0;
}



int CSqMimetic::setAttachment( const sqbind::stdString &sName, const sqbind::stdString &sType, sqbind::CSqBinary *pData )
{
	// std::string( pData->Ptr(), pData->getUsed() )

	if ( !sName.length() || !sType.length() || !pData )
		return 0;

	/*
		1) me is multipart:
			add the attach to me as the last part
		2) me is not multipart
			create a multipart/mixed with me and the attach childs
			and put it in the same level/position of me
		3) me is not multipart and is the top level entity
			same as 2) but move all me fields to the new top-level
	*/
	// Attachment* pA = new Attachment(fqn);
	mimetic::Attachment *pA = new mimetic::Attachment( sName.c_str(), mimetic::ContentType( sType.c_str() ) );
	if ( !pA || !pA->isValid() )
		return 0;

	if ( m_me.header().contentType().isMultipart() )
		m_me.body().parts().push_back( pA );

	else
	{
		mimetic::MimeEntity *mm = new mimetic::MultipartMixed;
		mm->body().parts().push_back( &m_me );
		mm->body().parts().push_back( pA );

/*		if( !isTopLevel )
		{   mimetic::MimeEntity *parent = *ph->begin();
			replace( parent->body().parts().begin(), parent->body().parts().end(), &m_me, mm );
		} // end if

		else 
		{
*/			// add cp fields here
			mimetic::Header::iterator bit, eit, pos;
			bit = m_me.header().begin(), eit = m_me.header().end();
			std::string name; // field name
			pos = mm->header().begin(); // insert before others
			for( ; bit != eit; ++bit )
			{
				name = bit->name();    
				std::transform( name.begin(), name.end(), 
					name.begin(), ::tolower );
				if( name.find( "content-" ) == 0 || name == "mime-version" )
					continue;
//				mm->header().insert( pos, *bit );
				mm->header().push_front( *bit );
//				return 1;
			}

//			m_me = *mm;
//		}

	}

	return 1;


/*
	m_me.body().parts().push_back( 
			&mimetic::Attachment( std::string( pData->Ptr(), pData->getUsed() ), mimetic::ContentType( sType.c_str() ) )
		);
	
	return 1;
*/
}

int CSqMimetic::getAttachment( const sqbind::stdString &sName, sqbind::CSqBinary *pData )
{

	return 0;
}

static int _buildStructure( mimetic::MimeEntity* pE, sqbind::CSqMulti *pM, int nCount = 0 )
{
	// Sanity checks
	if ( !pE || !pM || 128 < nCount )
		return 0;

	// Get header info
	mimetic::Header& h = pE->header();

	// Add info for this part
	sqbind::CSqMulti &p = (*pM)[ sqbind::oex2std( oexMks( nCount++ ) ) ];
	if ( h.hasField( oexT( "Message-ID" ) ) )
		p[ oexT( "message_id" ) ] = h.messageid().str().c_str();
	if ( h.hasField( mimetic::ContentId::label ) )
		p[ oexT( "content_id" ) ] = h.contentId().str().c_str();
	if ( h.hasField( mimetic::ContentType::label ) )
		p[ oexT( "type" ) ] = h.messageid().str().c_str();

	// Add sub parts
	mimetic::MimeEntityList& parts = pE->body().parts();
	for( mimetic::MimeEntityList::iterator mbit = parts.begin(); mbit != parts.end(); ++mbit )
		_buildStructure( *mbit, &p[ oexT( "parts" ) ], nCount );

	return nCount;
}

sqbind::CSqMulti CSqMimetic::getStructure()
{
	sqbind::CSqMulti m;

	_buildStructure( &m_me, &m );

	return m;
}
