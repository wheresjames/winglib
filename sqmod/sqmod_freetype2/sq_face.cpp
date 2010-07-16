// sq_face.cpp

#include "stdafx.h"

CFtFace::CFtFace()
{_STT();

	m_face = 0;

	setAngle( 0 );
//	setCharSize( 50 * 64, 0, 100, 0 );

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

	if ( !m_face )
		return 0;

	// Set font character params
	m_last_error = FT_Set_Char_Size( m_face, width, height, hrez, vrez );

	return m_last_error ? 0 : 1;
}

int CFtFace::LoadChar( int nChar )
{_STT();

	if ( !m_face )
		return 0;

	// Set transform
	FT_Set_Transform( m_face, &m_matrix, &m_pen );

	// Set font character params
	m_last_error = FT_Load_Char( m_face, nChar, FT_LOAD_RENDER );

	return m_last_error ? 0 : 1;
}
