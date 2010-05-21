// sqirr_node.h

#pragma once

class CSqirrNode
{
public:

	enum
	{
		/// Generic node type
		eTypeNone = 0,

		/// Camera type
		eTypeCamera,

		/// Mesh
		eTypeMesh,

		/// Animated mesh
		eTypeAnimatedMesh,

		/// Number of types
		eNumTypes
	};

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrNode )
	_SQBIND_CLASS_CTOR_END( CSqirrNode )

	/// Default constructor
	CSqirrNode() { m_p = 0; m_type = 0; }

	CSqirrNode( irr::scene::ISceneNode *p ) : m_type( 0 ), m_p( 0 ) { SetPtr( p, eTypeNone ); }
	CSqirrNode& operator = ( irr::scene::ISceneNode *p ) { SetPtr( p, eTypeNone ); return *this; }

	CSqirrNode( irr::scene::ICameraSceneNode *p ) : m_type( 0 ), m_p( 0 ) { SetPtr( p, eTypeCamera ); }
	CSqirrNode& operator = ( irr::scene::ICameraSceneNode *p ) { SetPtr( p, eTypeCamera ); return *this; }

	CSqirrNode( irr::scene::IAnimatedMeshSceneNode *p ) : m_type( 0 ), m_p( 0 ) { SetPtr( p, eTypeAnimatedMesh ); }
	CSqirrNode& operator = ( irr::scene::IAnimatedMeshSceneNode *p ) { SetPtr( p, eTypeAnimatedMesh ); return *this; }

	CSqirrNode( irr::scene::IMeshSceneNode *p ) : m_type( 0 ), m_p( 0 ) { SetPtr( p, eTypeMesh ); }
	CSqirrNode& operator = ( irr::scene::IMeshSceneNode *p ) { SetPtr( p, eTypeMesh ); return *this; }

	void SetPtr( irr::scene::ISceneNode *p, int t ) 
	{
		Destroy(); 
		if ( p ) 
		{ 
			m_p = p; 
			p->grab(); 
			m_type = t; 
		} 
	}

	~CSqirrNode() { Destroy(); }
	void Destroy() { if ( m_p ) { m_p->drop(); m_p = 0; } }

	// Copy semantics
	CSqirrNode( const CSqirrNode &r ) : m_type( 0 ), m_p( 0 ) { SetPtr( r.m_p, r.m_type ); }
	CSqirrNode& operator = ( const CSqirrNode &r ) { SetPtr( r.m_p, r.m_type ); return *this; }

	bool operator == ( const CSqirrNode &r ) { return r.m_p == m_p; }
	int IsEqual ( CSqirrNode *r ) { if ( !r ) return 0; return ( r->m_p == m_p ) ? 1 : 0; }

	irr::scene::ISceneNode* Ptr() { return m_p; }
	irr::scene::ISceneNode& Obj() { return *m_p; }
	int GetNodeType() { return m_type; }

	CSqirrNode* Node() { return this; }

public:

	void UpdateAbsolutePosition()
	{   if ( !m_p ) return;
		m_p->updateAbsolutePosition();
	}

	void AddAnimator( CSqirrAnimator &ani )
	{	if ( !m_p || !ani.Ptr() ) return;
		m_p->addAnimator( ani.Ptr() );
	}

	void SetPosition( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		m_p->setPosition( v.Obj() );
		m_p->updateAbsolutePosition();
	}

	CSqirrVector3d GetPosition()
	{   if ( !m_p ) return CSqirrVector3d();
		return m_p->getPosition();
	}

	void SetRotation( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		m_p->setRotation( v.Obj() );
	}

	CSqirrVector3d GetRotation()
	{   if ( !m_p ) return CSqirrVector3d();
		return m_p->getRotation();
	}

	void SetTexture( long lMaterial, CSqirrTexture &st )
	{
		if ( !m_p ) return;

		if ( 0 <= lMaterial )
			m_p->setMaterialTexture( lMaterial, st.Ptr() );

		else for ( unsigned int i = 0; i < m_p->getMaterialCount(); i++ )
			m_p->setMaterialTexture( i, st.Ptr() );
	}

	void FaceTarget( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		irr::core::vector3df rot, dif = m_p->getPosition() - v.Obj();
		rot.Y = atan2( dif.X, dif.Z ) * 180.f / irr::core::PI;
		rot.X = -atan2( dif.Y, sqrt( dif.X * dif.X + dif.Z * dif.Z ) ) * 180.f / irr::core::PI;
		m_p->setRotation( rot );
	}

	void FaceTargetY( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		irr::core::vector3df rot, dif = m_p->getPosition() - v.Obj();
		rot.Y = atan2( dif.X, dif.Z ) * 180.f / irr::core::PI;
		m_p->setRotation( rot );
	}

	void FaceTargetX( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		irr::core::vector3df rot, dif = m_p->getPosition() - v.Obj();
		rot.X = -atan2( dif.Y, sqrt( dif.X * dif.X + dif.Z * dif.Z ) ) * 180.f / irr::core::PI;
		m_p->setRotation( rot );
	}

	void Move( CSqirrVector3d &v )
	{
		if ( !m_p ) return;

		irr::core::matrix4 m;
		m.setRotationDegrees( m_p->getRotation() );
		m.transformVect( v.Obj() );

		m_p->setPosition( m_p->getPosition() + v.Obj() );

		if ( eTypeCamera == m_type )
			( (irr::scene::ICameraSceneNode*)m_p )->setTarget(
				( (irr::scene::ICameraSceneNode*)m_p )->getTarget() + v.Obj() );

		m_p->updateAbsolutePosition();
	}

	void Point( CSqirrVector3d &a, CSqirrVector3d &b, float scale )
	{
		if ( !m_p )
			return;

		// Get mid point
		a.Obj().X += ( b.Obj().X - a.Obj().X ) / 2;
		a.Obj().Y += ( b.Obj().Y - a.Obj().Y ) / 2;
		a.Obj().Z += ( b.Obj().Z - a.Obj().Z ) / 2;

		// Position
		Move( a );
		FaceTarget( b );

		// Need scale to be set?
		if ( scale )
		{	CSqirrVector3d s  = GetScale();
			s.Obj().Y = a.Distance( b ) * scale;
			SetScale( s );
		} // end if
	}

	CSqirrVector3d GetExtent()
	{   if ( !m_p ) return CSqirrVector3d();
		return m_p->getBoundingBox().getExtent();
	}

    CSqirrVector3d GetAbsoluteExtent()
	{   if ( !m_p ) return CSqirrVector3d();
		return m_p->getTransformedBoundingBox().getExtent();
	}

	void SetScale( CSqirrVector3d &v )
	{   if ( !m_p ) return;
		m_p->setScale( v.Obj() );
	}

	CSqirrVector3d GetScale()
	{   if ( !m_p ) return CSqirrVector3d();
		return m_p->getScale();
	}

	void SetMaterialFlag( long lFlag, long lVal )
	{   if ( !m_p ) return;
		m_p->setMaterialFlag( (irr::video::E_MATERIAL_FLAG)lFlag, lVal ? true : false );
	}

	void SetMaterialType( long lType )
	{   if ( !m_p ) return;
		m_p->setMaterialType( (irr::video::E_MATERIAL_TYPE)lType );
	}

	void SetParent( CSqirrNode &n )
	{   if ( m_p )
			m_p->setParent( n.Ptr() );
	}

	void SetVisible( int bVisible )
	{   if ( m_p )
			m_p->setVisible( bVisible ? true : false );
	}

	int IsVisible()
	{   if ( !m_p ) return 0;
		return m_p->isVisible();
	}

	void SetDebugDataVisible( int lEds )
	{   if ( !m_p ) return;
		m_p->setDebugDataVisible( (irr::scene::E_DEBUG_SCENE_TYPE)lEds );
	}

	void SetID( int id )
	{   if ( !m_p ) return;
		m_p->setID( id );
	}

	int GetID()
	{   if ( !m_p ) return 0;
		return m_p->getID();
	}

	int IsValid()
	{	return m_p ? 1 : 0; }

    void SetAbsolutePosition( CSqirrVector3d &x_v )
	{   if ( !m_p ) return;

        irr::scene::ISceneNode *pParent =  m_p->getParent();
        if ( !pParent ) m_p->setPosition( x_v.Obj() );
        else
        {   irr::core::vector3df v( x_v.Obj() - pParent->getAbsolutePosition() );
            pParent->getAbsoluteTransformation().inverseRotateVect( v );
            m_p->setPosition( v ); 
        } // end else
        m_p->updateAbsolutePosition();
    }

    CSqirrVector3d GetAbsolutePosition()
    {   if ( !m_p ) return CSqirrVector3d();
        return m_p->getAbsolutePosition();
    }

	CSqirrVector3d GetAbsoluteCenter()
	{   if ( !m_p ) return CSqirrVector3d();
		m_p->updateAbsolutePosition();
		CSqirrVector3d pos = m_p->getAbsolutePosition() + ( m_p->getBoundingBox().getCenter() * m_p->getScale() );
		return pos;
	}

	void SetAbsoluteCenter( CSqirrVector3d &x_v )
	{   if ( !m_p ) return;

		irr::scene::ISceneNode *pParent =  m_p->getParent();
		if ( !pParent ) SetCenter( x_v );
		else
		{   CSqirrVector3d v( x_v.Obj() - pParent->getAbsolutePosition() );
			pParent->getAbsoluteTransformation().inverseRotateVect( v.Obj() );                
			SetCenter( v ); 
		} // end else
		m_p->updateAbsolutePosition();
	}

    CSqirrVector3d GetCenter()
    {   if ( !m_p ) return CSqirrVector3d();
        return m_p->getPosition() + ( m_p->getBoundingBox().getCenter() * m_p->getScale() );
    }

    void SetCenter( CSqirrVector3d &v )
    {   if ( !m_p ) return;
        m_p->setPosition( v.Obj() - ( m_p->getBoundingBox().getCenter() * m_p->getScale() ) );
        m_p->updateAbsolutePosition();
    }

	void ConvertAngleToNormalXY( const irr::core::vector3df &a, irr::core::vector3df &n )
	{	irr::core::matrix4 m;
		m.setRotationDegrees( a );
		n.X = n.Y = 0; n.Z = -1;
		m.transformVect( n );
	}

	void ConvertNormalToAngleXY( const irr::core::vector3df &n, irr::core::vector3df &a )
	{   a.Y = atan2( n.X, n.Z ) * 180.f / irr::core::PI;
		a.X = -atan2( n.Y, sqrt( n.X * n.X + n.Z * n.Z ) ) * 180.f / irr::core::PI;
		a.Z = 0;
	}

	void GetPivotAngle( CSqirrVector3d &v, CSqirrVector3d &a )
	{	if ( !m_p ) return;
		ConvertNormalToAngleXY( v.Obj() - m_p->getPosition(), a.Obj() );
	}

	float GetPivotDist( CSqirrVector3d &v )
	{	if ( !m_p ) return 0;
		irr::core::line3df l( v.Obj(), m_p->getPosition() );
		return (float)l.getLength();
	}

	void SetPivot( CSqirrVector3d &s, CSqirrVector3d &a, float fDist )
	{	if ( !m_p ) return;
		irr::core::vector3df n;
		ConvertAngleToNormalXY( a.Obj(), n );
		m_p->setPosition( s.Obj() + ( n * fDist ) );
		m_p->updateAbsolutePosition();
	}

protected:

	int							m_type;

	irr::scene::ISceneNode    	*m_p;

};

_SQBIND_DECLARE_INSTANCE( CSqirrNode, CSqirrNode );

