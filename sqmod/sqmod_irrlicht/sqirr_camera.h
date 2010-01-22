// sqirr_camera.h

#pragma once

class CSqirrCamera : public CSqirrNode
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrCamera )
	_SQBIND_CLASS_CTOR_END( CSqirrCamera )

	CSqirrCamera() {}
	CSqirrCamera( irr::scene::ISceneNode *p ) { SetPtr( p, eTypeNone ); }
	CSqirrCamera& operator = ( irr::scene::ISceneNode *p ) { SetPtr( p, eTypeNone ); return *this; }
	CSqirrCamera( irr::scene::ICameraSceneNode *p ) { SetPtr( p, eTypeCamera ); }
	CSqirrCamera& operator = ( irr::scene::ICameraSceneNode *p ) { SetPtr( p, eTypeCamera ); return *this; }

	CSqirrNode* node()
	{
		return (CSqirrNode*)this;
	}

	void UpdateCamera()
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->OnRegisterSceneNode();
	}

	void SetTarget( CSqirrVector3d &v )
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->setTarget( v.Obj() );
	}

	CSqirrVector3d GetTarget()
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return CSqirrVector3d();

		return ( (irr::scene::ICameraSceneNode*)m_p )->getTarget();
	}

	void SyncTargetToRotation()
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		irr::core::vector3df p = m_p->getPosition();

		irr::core::matrix4 m;
		irr::core::vector3df ptTarget( 0, 0, -1 );

		m.setRotationDegrees( m_p->getRotation() );
		m.transformVect( ptTarget );
		ptTarget += p;

		( (irr::scene::ICameraSceneNode*)m_p )->setTarget( ptTarget );

	}

	void SetFov( float fFov )
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->setFOV( fFov );
	}

	void SetAspectRatio( float fRatio )
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->setAspectRatio( fRatio );
	}

	/**
		\param [in] fLens       -   Lens size in mm
		\param [in] lCcdWidth   -   CCD width in mm
		\param [in] lCcdHeight  -   CCD height in mm

										CCD       Width   Height
										-----------------------
										1/4     - 3.2   x 2.4
										1/3.6   - 4     x 3
										1/3.2   - 4.536 x 3.416
										1/3     - 4.8   x 3.6
										1/2.7   - 5.27  x 3.96
										1/2     - 6.4   x 4.8
										1/1.8   - 7.176 x 5.319
										2/3     - 8.8   x 6.6
										1       - 12.8  x 9.6
										4/3     - 18    x 13.5
	*/
	void SetLens( float fLens, float fCcdWidth, float fCcdHeight )
	{
		if ( !fLens )
			return;

		// Calculate field of view
		double x_fov = atan( fCcdWidth / fLens );
		double y_fov = atan( fCcdHeight / fLens );

		// Set fov and aspect ratio
		SetFov( (float)x_fov );
		SetAspectRatio( (float)( x_fov / y_fov ) );
	}


	void SetFarValue( long lValue )
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->setFarValue( (float)lValue );
	}

	void SetNearValue( long lValue )
	{
		if ( !m_p || GetNodeType() != eTypeCamera )
			return;

		( (irr::scene::ICameraSceneNode*)m_p )->setNearValue( (float)lValue );
	}


};

_SQBIND_DECLARE_INSTANCE( CSqirrCamera, CSqirrCamera );

