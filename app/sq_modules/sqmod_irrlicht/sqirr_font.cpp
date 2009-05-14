// sqirr_font.cpp

#include "stdafx.h"

// To make lzw happy
#if defined( DEBUG )
	int z_verbose;
	void z_error(char *m) {}
#endif

void CSqirrFont::Destroy()
{
	if ( m_font )
	{	OexAllocDelete( m_font );
		m_font = oexNULL;
	} // end if
}

int CSqirrFont::set( int size, const sqbind::stdString &sFont )
{
	// Lose old font
	Destroy();

	// Create new font
	m_font = OexAllocConstruct< FTExtrudeFont >( sFont.c_str() );
	if ( !m_font || m_font->Error() )
	{	oexERROR( 0, oexMks( oexT( "Unable to create font" ), sFont.c_str() ) );
		return 0;
	} // end if

	// Set font face size
	if ( !m_font->FaceSize( size ) )
	{	oexERROR( 0, oexMks( oexT( "Failed to set font face size of " ), size, oexT( " : " ), sFont.c_str() ) );
		return 0;
	} // end if

	return 1;
}
