// sq_physics.cpp

#include "stdafx.h"

const float CSqirrPhysics::c_fGravity = -9.80665f;

CSqirrPhysics::CSqirrPhysics()
{
}

CSqirrPhysics::~CSqirrPhysics()
{
	Destroy();
}

void CSqirrPhysics::Destroy()
{
#if !defined( OEX_NOBULLET )

	m_memPhysicsWorld.Delete();
	m_memSolver.Delete();
	m_memBroadphase.Delete();
	m_memCollisionDispatcher.Delete();
	m_memCollisionConfig.Delete();

#endif
}

int CSqirrPhysics::Init()
{
#if defined( OEX_NOBULLET )
	return 0;
#else

	Destroy();

	const int maxProxies = 32766;
	const int maxOverlap = 65535;

	// Create default configuration	
	if ( !m_memCollisionConfig.Construct().Ptr() )
	{	oexERROR( 0, "Out of memory allocating btDefaultCollisionConfiguration" ); 
		Destroy(); return 0;
	} // end if  

	if ( !m_memCollisionDispatcher.Construct( *m_memCollisionConfig ).Ptr() )
	{	oexERROR( 0, "Out of memory allocating btCollisionDispatcher" ); 
		Destroy(); return 0;
	} // end if  

	btVector3 worldAabbMin( -10000, -10000, -10000 );
	btVector3 worldAabbMax( 10000, 10000, 10000 );
	if ( !m_memBroadphase.Construct( btVector3( -10000, -10000, -10000 ),
									 btVector3( 10000, 10000, 10000 ),
									 maxProxies ).Ptr() )
	{	oexERROR( 0, "Out of memory allocating btAxisSweep3" ); 
		Destroy(); return 0;
	} // end if  

	if ( !m_memSolver.Construct().Ptr() )
	{	oexERROR( 0, "Out of memory allocating btSequentialImpulseConstraintSolver" ); 
		Destroy(); return 0;
	} // end if  

	if ( !m_memPhysicsWorld.Construct(  *m_memCollisionDispatcher, *m_memBroadphase, 
										*m_memSolver, *m_memCollisionConfig ).Ptr() )
	{	oexERROR( 0, "Out of memory allocating btDiscreteDynamicsWorld" ); 
		Destroy(); return 0;
	} // end if  

	// Set gravity
	m_memPhysicsWorld->setGravity( btVector3( 0, c_fGravity, 0 ) ); 

	return 1;
#endif
}

// new btDefaultCollisionConfiguration();

int CSqirrPhysics::Update( float fElapsed )
{
#if defined( OEX_NOBULLET )
	return 0;
#else

	if ( !m_memPhysicsWorld.Ptr() )
		return 0;

	m_memPhysicsWorld->stepSimulation( fElapsed, 1 );

	m_memPhysicsWorld->updateAabbs();

	return 1;

#endif
}

