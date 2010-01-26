// sqirr_color.h

#pragma once

class CSqirrColor
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrColor )
		_SQBIND_CLASS_CTOR( CSqirrColor, 3 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ), sa.GetInt( 4 ), 255 )
		_SQBIND_CLASS_CTOR( CSqirrColor, 4 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ), sa.GetInt( 4 ), sa.GetInt( 5 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrColor )

	/// Default constructor
	CSqirrColor() {}
	CSqirrColor( int r, int g, int b, int a ) : m_col( a, r, g, b ) {}
	CSqirrColor( const irr::video::SColor &c ) : m_col( c ) {}
	~CSqirrColor() {}

	// Copy semantics
	CSqirrColor( const CSqirrColor &r ) { m_col = r.m_col; }
	CSqirrColor& operator =( const CSqirrColor &r ) { m_col = r.m_col; return *this; }
	irr::video::SColor& Obj() { return m_col; }
	const irr::video::SColor& c_Obj() const { return m_col; }

	int r() { return m_col.getRed(); }
	int g() { return m_col.getGreen(); }
	int b() { return m_col.getBlue(); }
	int a() { return m_col.getAlpha(); }

private:

	irr::video::SColor		m_col;

}; _SQBIND_DECLARE_INSTANCE( CSqirrColor, CSqirrColor );


class CSqirrColorf
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrColorf )
		_SQBIND_CLASS_CTOR( CSqirrColorf, 3 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ), 1.f )
		_SQBIND_CLASS_CTOR( CSqirrColorf, 4 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ), sa.GetFloat( 5 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrColorf )

	/// Default constructor
	CSqirrColorf() {}
	CSqirrColorf( float r, float g, float b, float a ) : m_col( r, g, b, a ) {}
	CSqirrColorf( irr::video::SColorf c ) : m_col( c ) {}
	~CSqirrColorf() {}

	// Copy semantics
	CSqirrColorf( const CSqirrColorf &r ) { m_col = r.m_col; }
	CSqirrColorf& operator =( const CSqirrColorf &r ) { m_col = r.m_col; return *this; }
	irr::video::SColorf& Obj() { return m_col; }
	const irr::video::SColorf& c_Obj() const { return m_col; }

	float r() { return m_col.r; }
	float g() { return m_col.g; }
	float b() { return m_col.b; }
	float a() { return m_col.a; }

private:

	irr::video::SColorf		m_col;

}; _SQBIND_DECLARE_INSTANCE( CSqirrColorf, CSqirrColorf );
