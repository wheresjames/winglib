// sqirr_line.h

#pragma once

class CSqirrLine
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrLine )
//		_SQBIND_CLASS_CTOR( CSqirrLine, 3 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ) )
	_SQBIND_CLASS_CTOR_END( CSqirrLine )

	/// Default constructor
	CSqirrLine() {}
	CSqirrLine( irr::core::vector3df start, irr::core::vector3df end ) : m_start( start ), m_end( end ) {}
	~CSqirrLine() {}


	// Copy semantics
	CSqirrLine( const CSqirrLine &r ) { m_start = r.m_start; m_end = r.m_end; }
	CSqirrLine& operator =( const CSqirrLine &r ) { m_start = r.m_start; m_end = r.m_end; return *this; }

	void set( CSqirrVector3d &start, CSqirrVector3d &end )
	{
		m_start = start;
		m_end = end;
	}

	CSqirrVector3d& start() { return m_start; }
	CSqirrVector3d& end() { return m_end; }

	void inflate( float v ) 
	{
		m_start.inflate( v ); 
		m_end.inflate( v );
	}

	/// Creates line from a point and rotation
    void FromPointAndRotation( CSqirrVector3d &pt, CSqirrVector3d &rot, float len ) 
	{
		// Save start
		m_start = pt;

        irr::core::matrix4 m;
        m.setRotationDegrees( rot.Obj() );

		// Convert angle to normal
		irr::core::vector3df n( 0, 0, -1 );
        m.transformVect( n );

		// Calculate end point
		m_end.Obj() = pt.Obj() + n * len;

	}

    /// Gets the line rotation
	void GetRotation( CSqirrVector3d &a )
	{	irr::core::line3df l( m_start.x(), m_start.y(), m_start.z(),
							  m_end.x(), m_end.y(), m_end.z() );
		irr::core::vector3df n = l.getVector().normalize();
        a.Obj().Y = atan2( n.X, n.Z ) * 180.f / irr::core::PI;
        a.Obj().X = -atan2( n.Y, sqrt( n.X * n.X + n.Z * n.Z ) ) * 180.f / irr::core::PI;
        a.Obj().Z = 0;
	}

    /// Gets the line vector
	void GetVector( CSqirrVector3d &v )
	{	irr::core::line3df l( m_start.x(), m_start.y(), m_start.z(),
							  m_end.x(), m_end.y(), m_end.z() );
		v.Obj() = l.getVector();
	}

    /// Gets the line normal
	void GetNormal( CSqirrVector3d &n )
	{	irr::core::line3df l( m_start.x(), m_start.y(), m_start.z(),
							  m_end.x(), m_end.y(), m_end.z() );
		n.Obj() = l.getVector().normalize();
	}

	/// Returns the line length
	float GetLength()
	{	return ( m_end.Obj() - m_start.Obj() ).getLength(); }

private:

	CSqirrVector3d			m_start;
	CSqirrVector3d			m_end;
};

_SQBIND_DECLARE_INSTANCE( CSqirrLine, CSqirrLine );

