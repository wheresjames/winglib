// sq_physics.h

#pragma once

#if !defined( OEX_NOBULLET )
//#	include "btBulletCollisionCommon.h"
#	include "btBulletDynamicsCommon.h"
#endif

class CSqirrPhysics
{

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrPhysics )
	_SQBIND_CLASS_CTOR_END( CSqirrPhysics )

	/// Default constructor
	CSqirrPhysics();

	~CSqirrPhysics();

	// Copy semantics
	CSqirrPhysics( const CSqirrPhysics &r ) {}
	CSqirrPhysics& operator =( const CSqirrPhysics &r ) { return *this; }

	/// Updates physics world
	int Update( float fElapsed );


private:

#if !defined( OEX_NOBULLET )

	/// Bullet physics
	btDiscreteDynamicsWorld			*m_pPhysicsWorld;

#endif

};

_SQBIND_DECLARE_INSTANCE( CSqirrPhysics, CSqirrPhysics );

