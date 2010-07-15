// sq_face.cpp

#include "stdafx.h"

CFtFace::CFtFace()
{_STT();

	setAngle( 0 );
	setCharSize( 50 * 64, 0, 100, 0 );

}

CFtFace::~CFtFace() 
{
	Destroy(); 
}


void CFtFace::Destroy()
{_STT();

	FT_Done_Face( m_face );
}

void CFtFace::setAngle( double d )
{
	m_dAngle = d; 
	m_matrix.xx = (FT_Fixed)( cos( m_dAngle ) * 0x10000L );
	m_matrix.xy = (FT_Fixed)(-sin( m_dAngle ) * 0x10000L );
	m_matrix.yx = (FT_Fixed)( sin( m_dAngle ) * 0x10000L );
	m_matrix.yy = (FT_Fixed)( cos( m_dAngle ) * 0x10000L );
}


int CFtFace::setCharSize( int width, int height, int hrez, int vrez )
{_STT();

	// Set font character params
	m_last_error = FT_Set_Char_Size( m_face, width, height, hrez, vrez );

	return m_last_error ? 0 : 1;
}