// sqirr_font.cpp

#include "stdafx.h"

//#include <GL/gl.h>

// To make lzw happy
#if defined( DEBUG )
//	int z_verbose;
//	void z_error(char *m) {}
#endif

void CSqirrFont::Destroy()
{
//	if ( m_font )
	{//	OexAllocDelete( m_font );
//		m_font = oexNULL;
	} // end if
}

int CSqirrFont::set( int size, const sqbind::stdString &sFont )
{
	// Lose old font
	Destroy();

	// Create new font
//	m_font = OexAllocConstruct< FTExtrudeFont >( sFont.c_str() );
//	if ( !m_font || m_font->Error() )
//	{	oexERROR( 0, oexMks( oexT( "Unable to create font " ), sFont.c_str() ) );
//		Destroy();
//		return 0;
//	} // end if

	// Set font face size
//	if ( !m_font->FaceSize( size ) )
//	{	oexERROR( 0, oexMks( oexT( "Failed to set font face size of " ), size, oexT( " : " ), sFont.c_str() ) );
//		Destroy();
//		return 0;
//	} // end if

//	m_font->FaceSize(80);
//	m_font->Depth(10);
//	m_font->Outset(0, 3);
//	m_font->CharMap(ft_encoding_unicode);


	return 1;
}

int CSqirrFont::Render( const sqbind::stdString &sText )
{
//	if ( !m_font )
//		return 0;



//	return 1;

	float n = 0;
    float front_ambient[4]  = { 0.7, 0.7, 0.7, 0.0 };

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_DEPTH_TEST);

//	glPushMatrix();


//        glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
//        glColorMaterial(GL_FRONT, GL_DIFFUSE);
//        glTranslatef(0.0, 0.0, 20.0);
//        glRotatef(n / 1.11, 0.0, 1.0, 0.0);
//        glRotatef(n / 2.23, 1.0, 0.0, 0.0);
//        glRotatef(n / 3.17, 0.0, 0.0, 1.0);
//        glTranslatef(-260.0, -0.2, 0.0);
//        glColor3f(1.0, 1.0, 1.0);

//        m_font->Render( sText.c_str() );
//    glPopMatrix();

	return 1;

}
