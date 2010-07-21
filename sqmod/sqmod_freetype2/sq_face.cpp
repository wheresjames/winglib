// sq_face.cpp

#include "stdafx.h"

#define CFtFace_FONT_SCALE 0x10000L

CFtFace::CFtFace()
{_STT();

	m_face = 0;
	m_pen.x = 0;
	m_pen.y = 0;
	setAngle( 0 );

	m_red = 255;
	m_green = 255;
	m_blue = 255;

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
	m_matrix.xx = (FT_Fixed)( cos( m_dAngle ) * CFtFace_FONT_SCALE );
	m_matrix.xy = (FT_Fixed)(-sin( m_dAngle ) * CFtFace_FONT_SCALE );
	m_matrix.yx = (FT_Fixed)( sin( m_dAngle ) * CFtFace_FONT_SCALE );
	m_matrix.yy = (FT_Fixed)( cos( m_dAngle ) * CFtFace_FONT_SCALE );
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

			if ( glyph->root.advance.x )
				_w += abs( ( glyph->root.advance.x ) / CFtFace_FONT_SCALE );

			else if ( gbox.xMax > gbox.xMin )
				_w += gbox.xMax - gbox.xMin;

			if ( glyph->root.advance.y )
				_h += abs( ( glyph->root.advance.y ) / CFtFace_FONT_SCALE );

			else if ( gbox.yMax > gbox.yMin )
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

	if ( !width )
		width = sz.getX() + sEol.length();

	if ( !height )
		height = sz.getY();
	
	sqbind::stdString s;
	s.resize( width * height, sChars[ 0 ] );

	long x = 0, y = 0;
	for ( long i = 0; i < sStr.length(); i++ )
	{
		FT_BitmapGlyph glyph = getCharGlyph( sStr[ i ] );
		if ( glyph )
		{
			// Draw the character
			Char2Ascii( &x, &y, width, &glyph->bitmap, &s, sChars, oexT( "" ) );

			// Next font position
			x += abs( ( glyph->root.advance.x ) / CFtFace_FONT_SCALE );
			y += abs( ( glyph->root.advance.y ) / CFtFace_FONT_SCALE );

		} // end if

	} // end for

	if ( sEol.length() )
		for ( long y = 0; y < height; y++ )
			for ( long i = 0; i < sEol.length(); i++ )
				s[ y * width + sz.getX() + i ] = sEol[ i ];

	return s;
}

int CFtFace::DrawImg( const sqbind::stdString &sStr, sqbind::CSqPos *pPos, sqbind::CSqImage *pImg, int flip )
{
	// Sanity checks
	if ( !pImg || 0 >= pImg->getWidth() || !pPos || 0 >= pImg->getHeight() || !sStr.length() )
		return 0;

	// Get pixel buffer
	sqbind::CSqBinary buf;
	if ( !pImg->refPixels( &buf ) )
		return 0;

	// Draw the font
	return DrawBin( sStr, pPos, &buf, 0, &pImg->getSize(), pImg->getScanWidth(), flip );
}

int CFtFace::DrawBin( const sqbind::stdString &sStr, sqbind::CSqPos *pPos, sqbind::CSqBinary *pBin, int fmt, sqbind::CSqSize *pSize, int sw, int flip )
{
	// Sanity checks
	if ( !pBin || 0 >= pBin->getUsed() || !pPos || !pSize || 0 >= pSize->getX() || 0 >= pSize->getY() || !sStr.length() )
		return 0;

	// Get string size
	sqbind::CSqSize sz;
	if ( !CalcSize( sStr, &sz ) )
		return 0;

	int x = pPos->getX(), y = pPos->getY();
	for ( long i = 0; i < sStr.length(); i++ )
	{
		FT_BitmapGlyph glyph = getCharGlyph( sStr[ i ] );
		if ( glyph )
		{
			// Draw the character
			DrawCharBin( glyph, x, y, pBin, fmt, pSize->getX(), pSize->getY(), sw, flip );

			// Next font position
			x += abs( ( glyph->root.advance.x ) / CFtFace_FONT_SCALE );
			y += abs( ( glyph->root.advance.y ) / CFtFace_FONT_SCALE );

		} // end if

	} // end for


	return 1;
}

int CFtFace::DrawCharBin( FT_BitmapGlyph g, int x, int y, sqbind::CSqBinary *pBin, int fmt, int w, int h, int sw, int flip )
{
	if ( !pBin || 0 >= pBin->getUsed() || 0 >= w || 0 >= h || !g )
		return 0;

	// Source
	int bw = g->bitmap.width;
	int bh = g->bitmap.rows;
	unsigned char *ps = g->bitmap.buffer;

	// Add character offsets
	x += g->left;
	y += flip ? -g->top : g->top;

	// Get pointer to buffer
	sqbind::CSqBinary::t_byte *p = pBin->_Ptr();
	sqbind::CSqBinary::t_size max = pBin->getUsed();

	for ( long _y = 0; _y < bh; _y++ )
	{
		for ( long _x = 0; _x < bw; _x++ )
		{
			// Get character
			int c = (int)(unsigned char)ps[ _y * bw + _x ];

			if ( c )
			{
				int py = y + _y;
				int px = x + _x;
				if ( 0 <= py && py <= h && 0 <= px && px <= w )
				{	int m;
					if ( !flip ) m = py * sw + ( px * 3 );
					else m = ( h - py ) * sw + ( px * 3 );
					if ( 0 <= m && m < max )
					{	p[ m ] = ( m_blue * c ) >> 8;
						p[ m + 1 ] = ( m_green * c ) >> 8;
						p[ m + 2 ] = ( m_red * c ) >> 8;
					} // end if
				} // end if

			} // end if

		} // end for

	} // end for

	return 1;
}

