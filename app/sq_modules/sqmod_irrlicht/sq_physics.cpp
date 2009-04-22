// sq_physics.cpp

#include "stdafx.h"

#define SQ_GRAVITY -9.80665f

CSqirrPhysics::CSqirrPhysics()
{
	m_pPhysicsWorld = oexNULL;
}

CSqirrPhysics::~CSqirrPhysics()
{
	if ( oexCHECK_PTR( m_pPhysicsWorld ) )
		OexAllocDelete( m_pPhysicsWorld );

	m_pPhysicsWorld = oexNULL;
}


int CSqirrPhysics::Update( float fElapsed )
{
	if ( !m_pPhysicsWorld )
		return 0;

	m_pPhysicsWorld->stepSimulation( fElapsed, 1 );

	return 1;
}

