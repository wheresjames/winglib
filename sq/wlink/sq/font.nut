
_self.load_module( "freetype2", "" );

class CFont
{
	m_face = 0;
	m_ft = 0;
	m_y = 2;
	m_h = 0;
	m_lines = 0;

	function getLines() { return m_lines; }

	function Destroy()
	{	if ( m_face ) m_face.Destroy(), m_face = 0;
		if ( m_ft ) m_ft.Destroy(), m_ft = 0;
	}

	function Create( font_name, img_height )
	{
		m_ft = CFtLibrary();
		if ( m_ft.getLastError() )
		{	::_self.echo( "freetype2 failed to initialize : " + m_ft.getLastError() );
			return 0;
		} // end if

		m_face = CFtFace();
		if ( !m_ft.LoadFont( m_face, font_name, 0 ) )
		{	::_self.echo( "freetype2 failed to load font '" + font_name + "' : " + m_ft.getLastError() );
			return 0;
		} // end if

		// Set font size
		m_face.setCharSize( 10 * 64, 0, 100, 0 );

		local sz = CSqSize();
		m_face.CalcSize( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", sz );

		// Calculate metrics
		m_h = sz.getY() + 2;
		m_lines = img_height / m_h;

		return 1;
	}

	function Draw( center, img, txt )
	{
		if ( !m_ft || !m_face )
			return 0;

		local sz = CSqSize();
		m_face.CalcSize( txt, sz );

		local pix = CSqBinary();
		if ( !img.refPixels( pix ) )
			return 0;

		local y = 0;

		if ( center )
			y = ( ( img.getHeight() - sz.getY() ) / 2 ) + sz.getY();
		else
			m_y += m_h, y = m_y;

		m_face.CalcSize( txt, sz );
		m_face.setColor( 0, 255, 0 );
		m_face.DrawImg( txt, CSqPos( 4, y ), img, 1 );

		return 1;

	}

};

