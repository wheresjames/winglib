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

	t_GlyphMap::iterator it = m_mapGlyph.begin();
	while ( it != m_mapGlyph.end() )
		FT_Done_Glyph( (FT_Glyph)it->second ),
		m_mapGlyph.erase( it++ );


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

FT_Bitmap* CFtFace::getCharBmp( oex::oexTCHAR c )
{_STT();
	FT_BitmapGlyph glyph = getCharGlyph( c );
	return glyph ? &glyph->bitmap : 0;
}

FT_BitmapGlyph CFtFace::getCharGlyph( oex::oexTCHAR c )
{_STT();

	if ( !m_face )
		return 0;

	t_GlyphMap::iterator it = m_mapGlyph.find( c );
	if ( it != m_mapGlyph.end() )
		return it->second;

	// Set transform
	FT_Set_Transform( m_face, &m_matrix, &m_pen );

	// Set font character params
	m_last_error = FT_Load_Glyph( m_face, FT_Get_Char_Index( m_face, c ), FT_LOAD_DEFAULT );
	if ( m_last_error )
		return 0;

	// Copy the glyph
	FT_Glyph glyph;
	m_last_error = FT_Get_Glyph( m_face->glyph, &glyph );

	// Render to bitmap if needed
	if ( FT_GLYPH_FORMAT_BITMAP != glyph->format )
		if ( m_last_error = FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 ) )
			return 0;

	// Bitmap glyph
	m_mapGlyph[ c ] = (FT_BitmapGlyph)glyph;

	// Return the bitmap
	return m_mapGlyph[ c ];
}

int CFtFace::LoadChar( const sqbind::stdString &c )
{_STT();

	if ( !m_face || !c.length() )
		return 0;

	return getCharBmp( c[ 0 ] ) ? 1 : 0;
}

int CFtFace::CalcSize( const sqbind::stdString &s, sqbind::CSqSize *pSize )
{
	if ( !pSize ) 
		return 0;
	
	if ( !m_face )
	{	pSize->set( 0, 0 );
		return 0;
	} // end if

	FT_BBox gbox;
	FT_BitmapGlyph glyph;
	int _w = 0, _h = 0;

	for ( int i = 0; i < s.length(); i++ )
		if ( ( glyph = getCharGlyph( s[ i ] ) ) )
		{
			FT_Glyph_Get_CBox( (FT_Glyph)glyph, FT_GLYPH_BBOX_PIXELS, &gbox );

			if ( gbox.xMax > gbox.xMin )
				_w += gbox.xMax - gbox.xMin;

			if ( gbox.yMax > gbox.yMin )
				_h = oex::cmn::Max( _h, (int)( gbox.yMax - gbox.yMin ) );

		} // end if

	pSize->set( _w, _h );

	return _w;
}

int CFtFace::Char2Ascii( long *ox, long *oy, long tw, FT_Bitmap *pBmp, sqbind::stdString *sOut, const sqbind::stdString &sChars, const sqbind::stdString &sEol )
{
	// Sanity checks
	if ( !sOut || !m_face || !pBmp || sChars.length() < 2 )
		return 0;

	long w = pBmp->width;
	long h = pBmp->rows;
	long m = sChars.length() - 1;
	sqbind::stdString s;

	// Ensure string is at least as large as we need
	long _ox = ox ? *ox : 0, _oy = oy ? *oy : 0;
	long sw = ( _oy + h ) * ( _ox + w + sEol.length() );
	if ( sOut->length() < sw )
		sOut->resize( sw, sChars[ 0 ] ), 
		tw = _ox + w + sEol.length();

	for ( long y = 0; y < h; y++ )
	{
		for ( long x = 0; x < w; x++ )
		{
			// Get character
			long c = (int)(unsigned char)pBmp->buffer[ y * w + x ];

			// Set character
			(*sOut)[ ( _oy + y ) * tw + ( _ox + x ) ]
				= !c ? sChars[ 0 ] : sChars[ ( c * m >> 8 ) + 1 ];

		} // end for

		// Copy end of line string
		if ( sEol.length() )
			for ( long i = 0; i < sEol.length(); i++ )
				(*sOut)[ ( _oy + y ) * tw + ( _ox + w + i ) ] = sEol[ i ];

	} // end for

	// Add character width to output
	if ( ox )
	{
		*ox += w;

//		*ox += slot->advance.x >> 6

	} // end if

	return 1;
}

sqbind::stdString CFtFace::Str2Ascii( int width, int height, const sqbind::stdString &sStr, const sqbind::stdString &sChars, const sqbind::stdString &sEol )
{
	// Sanity checks
	if ( !m_face || sChars.length() < 2 )
		return sqbind::stdString();

	sqbind::CSqSize sz;
	if ( !CalcSize( sStr, &sz ) )
		return sqbind::stdString();

	long xm = 2;

	if ( !width )
		width = sz.getX() + sEol.length() + ( sStr.length() * xm );

	if ( !height )
		height = sz.getY();
	
	sqbind::stdString s;
	s.resize( width * height, sChars[ 0 ] );

	long x = 0, y = 0;
	for ( long i = 0; i < sStr.length(); i++ )
	{
		Char2Ascii( &x, &y, width, getCharBmp( sStr[ i ] ), &s, sChars, oexT( "" ) );

		x += xm;

	} // end for

	if ( sEol.length() )
		for ( long y = 0; y < height; y++ )
			for ( long i = 0; i < sEol.length(); i++ )
				s[ y * width + sz.getX() + ( sStr.length() * xm ) + i ] = sEol[ i ];

	return s;
}
