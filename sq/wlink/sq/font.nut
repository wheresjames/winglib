
_self.load_module( "freetype2", "" );

class CFont
{
	m_face = 0;
	m_ft = 0;

	function Destroy()
	{	if ( m_face ) m_face.Destroy(), m_face = 0;
		if ( m_ft ) m_ft.Destroy(), m_ft = 0;
	}

	function Create( font_name )
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
		m_face.setCharSize( 12 * 64, 0, 100, 0 );

		return 1;
	}

	function Draw( img, txt )
	{
		if ( !m_ft || !m_face )
			return 0;

		local sz = CSqSize();
		m_face.CalcSize( txt, sz );

		local pix = CSqBinary();
		if ( !img.refPixels( pix ) )
			return 0;

		m_face.CalcSize( txt, sz );
		m_face.setColor( 0, 128, 0 );
		m_face.DrawImg( txt, CSqPos( 2, 1 + ( ( img.getHeight() - sz.getY() ) / 2 ) + sz.getY() ), img, 1 );

		m_face.setColor( 255, 255, 255 );
		m_face.DrawImg( txt, CSqPos( 1, ( ( img.getHeight() - sz.getY() ) / 2 ) + sz.getY() ), img, 1 );

		return 1;

	}

};
