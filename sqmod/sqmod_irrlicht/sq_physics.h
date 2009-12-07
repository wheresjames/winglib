// sq_physics.h

#pragma once

#if !defined( OEX_NOBULLET )
//#	include "btBulletCollisionCommon.h"
#	include "btBulletDynamicsCommon.h"
#endif

class CSqirrPhysics
{
public:

	/// Gravity
	static const float c_fGravity;

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrPhysics )
	_SQBIND_CLASS_CTOR_END( CSqirrPhysics )

	/// Default constructor
	CSqirrPhysics();

	~CSqirrPhysics();

	// Copy semantics
	CSqirrPhysics( const CSqirrPhysics &r ) {}
	CSqirrPhysics& operator =( const CSqirrPhysics &r ) { return *this; }

	/// Initializes the physics engine
	int Init();

	/// Release physics resources
	void Destroy();

	/// Updates physics world
	int Update( float fElapsed );


private:

#if !defined( OEX_NOBULLET )

	/// Bullet physics
	oex::TMem< btDiscreteDynamicsWorld >				m_memPhysicsWorld;

	/// Configuration
	oex::TMem< btDefaultCollisionConfiguration >		m_memCollisionConfig;

	/// Collision dispatcher
	oex::TMem< btCollisionDispatcher >					m_memCollisionDispatcher;

	/// Axis sweep
	oex::TMem< btAxisSweep3 >							m_memBroadphase;

	/// Solver
	oex::TMem< btSequentialImpulseConstraintSolver >	m_memSolver;

#endif

};

_SQBIND_DECLARE_INSTANCE( CSqirrPhysics, CSqirrPhysics );

