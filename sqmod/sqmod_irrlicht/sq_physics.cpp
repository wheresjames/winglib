// sq_physics.cpp

#include "stdafx.h"

/*

	http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=30994


btCompoundShape* cShape = new btCompoundShape();

for(u32 i = 0;i < meshBufferCount;++i)
{
      IMeshBuffer* mb = mesh->getMeshBuffer(i);
      cShape->addChildShape(btTransform::getIdentity(),
         new btConvexHullShape(&((S3DVertex*)mb->getVertices())->Pos.X,
         mb->getVertexCount(), getVertexPitchFromType(mb->getVertexType())));   
} 
*/

const float CSqirrPhysics::c_fGravity = -9.80665f;

CSqirrPhysics::CSqirrPhysics()
{_STT();
}

CSqirrPhysics::~CSqirrPhysics()
{_STT();
	Destroy();
}

void CSqirrPhysics::Destroy()
{_STT();
#if !defined( OEX_NOBULLET )

	m_memPhysicsWorld.OexDelete();
	m_memSolver.OexDelete();
	m_memBroadphase.OexDelete();
	m_memCollisionDispatcher.OexDelete();
	m_memCollisionConfig.OexDelete();

#endif
}

int CSqirrPhysics::Init()
{_STT();
#if defined( OEX_NOBULLET )
	return 0;
#else

	Destroy();

	const int maxProxies = 32766;
//	const int maxOverlap = 65535;

	// Create default configuration
	if ( !m_memCollisionConfig.OexConstruct().Ptr() )
	{	oexERROR( 0, "Out of memory allocating btDefaultCollisionConfiguration" );
		Destroy(); return 0;
	} // end if

	if ( !m_memCollisionDispatcher.OexConstruct( *m_memCollisionConfig ).Ptr() )
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

	if ( !m_memSolver.OexConstruct().Ptr() )
	{	oexERROR( 0, "Out of memory allocating btSequentialImpulseConstraintSolver" );
		Destroy(); return 0;
	} // end if

	if ( !m_memPhysicsWorld.OexConstruct(  *m_memCollisionDispatcher, *m_memBroadphase,
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
{_STT();
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

