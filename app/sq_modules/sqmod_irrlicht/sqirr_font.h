// sqirr_font.h

#pragma once

class CSqirrFont
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrFont )
		_SQBIND_CLASS_CTOR( CSqirrFont, 2 ) ( sa.GetInt( 2 ), sa.GetString( 3 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrFont )

	/// Default constructor
	CSqirrFont() : m_font( 0 ) {}
	CSqirrFont( int size, const sqbind::stdString &sFont ) : m_font( 0 )
	{	set( size, sFont ); }
	~CSqirrFont() {}

	/// Releases font
	void Destroy();

	// Copy semantics
	CSqirrFont( const CSqirrFont &r ) {}
	CSqirrFont& operator =( const CSqirrFont &r ) { return *this; }

	/// Sets the specified font
	int set( int size, const sqbind::stdString &sFont );

	/// Renders the specified text
	int Render( const sqbind::stdString &sText );

private:

	// Pointer to font
	FTFont			*m_font;

};

_SQBIND_DECLARE_INSTANCE( CSqirrFont, CSqirrFont );
