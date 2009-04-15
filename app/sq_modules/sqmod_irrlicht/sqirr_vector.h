// sqirr_vector.h

#pragma once

class CSqirrVector3d
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrVector3d )
		_SQBIND_CLASS_CTOR( CSqirrVector3d, 3 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrVector3d )

	/// Default constructor
	CSqirrVector3d() {}
	CSqirrVector3d( float x, float y, float z ) : m_v( x, y, z ) {}
	CSqirrVector3d( irr::core::vector3df v ) : m_v( v ) {}
	~CSqirrVector3d() {}


	// Copy semantics
	CSqirrVector3d( const CSqirrVector3d &r ) { m_v = r.m_v; }
	CSqirrVector3d& operator =( const CSqirrVector3d &r ) { m_v = r.m_v; return *this; }
	const irr::core::vector3df& c_Obj() const { return m_v; }
	irr::core::vector3df& Obj() { return m_v; }

	void set( float x, float y, float z )
	{
		m_v.X = x;
		m_v.Y = y;
		m_v.Z = z;
	}

	float x() { return m_v.X; }
	float y() { return m_v.Y; }
	float z() { return m_v.Z; }

	void inflate( float v ) { m_v.X *= v; m_v.Y *= v; m_v.Z *= v; }

private:

	irr::core::vector3df    m_v;

};

_SQBIND_DECLARE_INSTANCE( CSqirrVector3d, CSqirrVector3d );


class CSqirrVector2d
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrVector2d )
		_SQBIND_CLASS_CTOR( CSqirrVector2d, 2 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrVector2d )

	/// Default constructor
	CSqirrVector2d() {}
	CSqirrVector2d( float x, float y ) : m_v( x, y ) {}
	CSqirrVector2d( irr::core::vector2df v ) : m_v( v ) {}
	~CSqirrVector2d() {}


	// Copy semantics
	CSqirrVector2d( const CSqirrVector2d &r ) { m_v = r.m_v; }
	CSqirrVector2d& operator =( const CSqirrVector2d &r ) { m_v = r.m_v; return *this; }
	const irr::core::vector2df& Obj() const { return m_v; }

	void set( float x, float y )
	{
		m_v.X = x;
		m_v.Y = y;
	}

	float x() { return m_v.X; }
	float y() { return m_v.Y; }

	void inflate( float v ) { m_v.X *= v; m_v.Y *= v; }

private:

	irr::core::vector2df    m_v;

};

_SQBIND_DECLARE_INSTANCE( CSqirrVector2d, CSqirrVector2d );


class CSqirrVertexArray
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrVertexArray )
	_SQBIND_CLASS_CTOR_END( CSqirrVertexArray )

	/// Default constructor
	CSqirrVertexArray() { m_len = 0; m_pv = 0; }
	CSqirrVertexArray( irr::video::S3DVertex *x_pv, long x_len ) { m_pv = x_pv; m_len = x_len; }
	~CSqirrVertexArray() {}


	// Copy semantics
	CSqirrVertexArray( const CSqirrVertexArray &r ) { m_len = r.m_len; m_pv = r.m_pv; }
	CSqirrVertexArray& operator =( const CSqirrVertexArray &r ) { m_len = r.m_len; m_pv = r.m_pv; return *this; }

	void set( long i, float x, float y, float z )
	{
		if ( !m_pv || i >= m_len )
			return;

		m_pv[ i ].Pos.X = x;
		m_pv[ i ].Pos.Y = y;
		m_pv[ i ].Pos.Z = z;
	}

	float x( long i )
	{
		if ( !m_pv || i >= m_len )
			return 0;
		return m_pv[ i ].Pos.X;
	}

	float y( long i )
	{
		if ( !m_pv || i >= m_len )
			return 0;
		return m_pv[ i ].Pos.Y;
	}

	float z( long i )
	{
		if ( !m_pv || i >= m_len )
			return 0;
		return m_pv[ i ].Pos.Z;
	}

	void inflate( float v )
	{
		for( long i = 0; i < m_len; i++ )
		{
			m_pv[ i ].Pos.X *= v;
			m_pv[ i ].Pos.Y *= v;
			m_pv[ i ].Pos.Z *= v;

		} // end for
	}

private:

	long                    m_len;
	irr::video::S3DVertex   *m_pv;

};

_SQBIND_DECLARE_INSTANCE( CSqirrVertexArray, CSqirrVertexArray );
