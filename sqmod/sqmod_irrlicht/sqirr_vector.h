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

	float& x() { return m_v.X; }
	float& y() { return m_v.Y; }
	float& z() { return m_v.Z; }

	void inflate( float v ) { m_v.X *= v; m_v.Y *= v; m_v.Z *= v; }

	float Distance( CSqirrVector3d &pt )
	{	float dx = pt.Obj().X - Obj().X;
		float dy = pt.Obj().Y - Obj().Y;
		float dz = pt.Obj().Z - Obj().Z;
		float dxy = sqrt( dx * dx + dy * dy );
		return sqrt( dxy * dxy + dz * dz );
	}

	int CalcTransitionSpeed( CSqirrVector3d &pt, CSqirrVector3d *speed, int steps, float pps )
	{
		if ( !speed )
			return 0;

		// points per step?
		if ( pps )
		{	steps = (int)( Distance( pt ) / pps );
			if ( steps < 1 )
				steps = 1;
		} // end if

		// Calculate appropriate speed per axis
		speed->Obj().X = ( pt.Obj().X - Obj().X ) / (float)steps;
		speed->Obj().Y = ( pt.Obj().Y - Obj().Y ) / (float)steps;
		speed->Obj().Z = ( pt.Obj().Z - Obj().Z ) / (float)steps;

		return 1;
	}

	int Transition( CSqirrVector3d &pt, CSqirrVector3d &speed, float min )
	{
		float dx = pt.Obj().X - Obj().X;
		float dy = pt.Obj().Y - Obj().Y;
		float dz = pt.Obj().Z - Obj().Z;

		if ( !dx && !dy && !dz )
			return 0;

		if ( dx < min && dx > -min )
			Obj().X = pt.Obj().X;
		else
			Obj().X += speed.Obj().X;

		if ( dy < min && dy > -min )
			Obj().Y = pt.Obj().Y;
		else
			Obj().Y += speed.Obj().Y;

		if ( dz < min && dz > -min )
			Obj().Z = pt.Obj().Z;
		else
			Obj().Z += speed.Obj().Z;

		return 1;
	}

	int ExpTransition( CSqirrVector3d &b, float div, float min )
	{
		float dx = b.Obj().X - Obj().X;
		float dy = b.Obj().Y - Obj().Y;
		float dz = b.Obj().Z - Obj().Z;

		if ( !dx && !dy && !dz )
			return 0;

		if ( div < 1 ) div = 1;

		if ( dx < min && dx > -min ) Obj().X = b.Obj().X;
		else Obj().X += dx / div;

		if ( dy < min && dy > -min ) Obj().Y = b.Obj().Y;
		else Obj().Y += dy / div;

		if ( dz < min && dz > -min ) Obj().Z = b.Obj().Z;
		else Obj().Z += dz / div;

		return 1;
	}


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

	float& x() { return m_v.X; }
	float& y() { return m_v.Y; }

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

class CSqirrRect2d
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrRect2d )
		_SQBIND_CLASS_CTOR( CSqirrRect2d, 4 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ), sa.GetFloat( 5 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrRect2d )

	/// Default constructor
	CSqirrRect2d() {}
	CSqirrRect2d( float l, float t, float r, float b ) : m_lt( l, t ), m_rb( r, b ) {}
	CSqirrRect2d( irr::core::vector2df lt, irr::core::vector2df rb ) : m_lt( lt ), m_rb( rb ) {}
	~CSqirrRect2d() {}


	// Copy semantics
	CSqirrRect2d( const CSqirrRect2d &r ) { m_lt = r.m_lt; m_rb = r.m_rb; }
	CSqirrRect2d& operator =( const CSqirrRect2d &r ) { m_lt = r.m_lt; m_rb = r.m_rb; return *this; }

	void set( float l, float t, float r, float b )
	{
		m_lt.X = l;
		m_lt.Y = t;
		m_rb.X = r;
		m_rb.Y = b;
	}

	float& top() { return m_lt.X; }
	float& left() { return m_lt.Y; }
	float& right() { return m_rb.X; }
	float& bottom() { return m_rb.Y; }

	irr::core::vector2df& ltObj() { return m_lt; }
	irr::core::vector2df& rbObj() { return m_rb; }

	CSqirrVector2d lt() { return m_lt; }
	CSqirrVector2d rb() { return m_rb; }

	void inflate( float v )
	{	m_lt.X *= v; m_lt.Y *= v; m_rb.X *= v; m_rb.Y *= v; }

private:

	irr::core::vector2df    m_lt;
	irr::core::vector2df    m_rb;
};

_SQBIND_DECLARE_INSTANCE( CSqirrRect2d, CSqirrRect2d );

class CSqirrRect3d
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrRect3d )
		_SQBIND_CLASS_CTOR( CSqirrRect3d, 6 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ),
											    sa.GetFloat( 5 ), sa.GetFloat( 6 ), sa.GetFloat( 7 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrRect3d )

	/// Default constructor
	CSqirrRect3d() {}
	CSqirrRect3d( float l, float t, float f, float r, float b, float bk ) : m_ltf( l, t, f ), m_rbb( r, b, bk ) {}
	CSqirrRect3d( irr::core::vector3df ltf, irr::core::vector3df rbb ) : m_ltf( ltf ), m_rbb( rbb ) {}
	~CSqirrRect3d() {}


	// Copy semantics
	CSqirrRect3d( const CSqirrRect3d &r ) { m_ltf = r.m_ltf; m_rbb = r.m_rbb; }
	CSqirrRect3d& operator =( const CSqirrRect3d &r ) { m_ltf = r.m_ltf; m_rbb = r.m_rbb; return *this; }

	void set( float l, float t, float f, float r, float b, float bk )
	{
		m_ltf.X = l;
		m_ltf.Y = t;
		m_ltf.Z = f;
		m_rbb.X = r;
		m_rbb.Y = b;
		m_rbb.Z = bk;
	}

	float& top() { return m_ltf.X; }
	float& left() { return m_ltf.Y; }
	float& front() { return m_ltf.Z; }
	float& right() { return m_rbb.X; }
	float& bottom() { return m_rbb.Y; }
	float& back() { return m_rbb.Z; }

	void inflate( float v )
	{	m_ltf.X *= v; m_ltf.Y *= v; m_ltf.Z *= v; m_rbb.X *= v; m_rbb.Y *= v; m_rbb.Z *= v; }

private:

	irr::core::vector3df    m_ltf;
	irr::core::vector3df    m_rbb;
};

_SQBIND_DECLARE_INSTANCE( CSqirrRect3d, CSqirrRect3d );

class CSqirrBoundingBox3d
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrBoundingBox3d )
		_SQBIND_CLASS_CTOR( CSqirrBoundingBox3d, 6 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ),
											    	   sa.GetFloat( 5 ), sa.GetFloat( 6 ), sa.GetFloat( 7 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrBoundingBox3d )

	/// Default constructor
	CSqirrBoundingBox3d() {}
	CSqirrBoundingBox3d( float l, float t, float f, float r, float b, float bk ) : m_bb( l, t, f, r, b, bk ) {}
	CSqirrBoundingBox3d( irr::core::aabbox3df bb ) : m_bb( bb ) {}
	~CSqirrBoundingBox3d() {}


	// Copy semantics
	CSqirrBoundingBox3d( const CSqirrBoundingBox3d &r ) { m_bb = r.m_bb; }
	CSqirrBoundingBox3d& operator =( const CSqirrBoundingBox3d &r ) { m_bb = r.m_bb; return *this; }

	void set( float l, float t, float f, float r, float b, float bk )
	{
		m_bb.MinEdge.X = l;
		m_bb.MinEdge.Y = t;
		m_bb.MinEdge.Z = f;
		m_bb.MaxEdge.X = r;
		m_bb.MaxEdge.Y = b;
		m_bb.MaxEdge.Z = bk;
	}

	float& top() { return m_bb.MinEdge.X; }
	float& left() { return m_bb.MinEdge.Y; }
	float& front() { return m_bb.MinEdge.Z; }
	float& right() { return m_bb.MaxEdge.X; }
	float& bottom() { return m_bb.MaxEdge.Y; }
	float& back() { return m_bb.MaxEdge.Z; }

	void inflate( float v )
	{
		m_bb.MinEdge.X *= v; m_bb.MinEdge.Y *= v; m_bb.MinEdge.Z *= v;
		m_bb.MaxEdge.X *= v; m_bb.MaxEdge.Y *= v; m_bb.MaxEdge.Z *= v;
	}

	irr::core::aabbox3df& Obj() { return m_bb; }

private:

	irr::core::aabbox3df		m_bb;

};

_SQBIND_DECLARE_INSTANCE( CSqirrBoundingBox3d, CSqirrBoundingBox3d );
