// sq_irrlicht.cpp

#include "stdafx.h"

CSqIrrlicht::~CSqIrrlicht()
{
	if ( m_pDevice )
	{	m_pDevice->closeDevice();
		m_pDevice->drop();
		m_pDevice = oexNULL;
	} // end if
}


int CSqIrrlicht::Init( int width, int height )
{

	irr::SIrrlichtCreationParameters param;	
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::s32>( width, height );
	param.DriverType = irr::video::EDT_OPENGL;
//	param.WindowId = (void*)reinterpret_cast<irr::s32>( GetHandle() );
	m_pDevice = irr::createDeviceEx( param );



//	m_pDevice = irr::createDevice( irr::video::EDT_OPENGL, irr::core::dimension2d<irr::s32>( 640, 480 ), 16,
//				   			  false, false, false, 0 );

	if ( !m_pDevice )
		oexERROR( 0, oexT( "Failed to create irrlicht device" ) );

	else
	{
//		m_pDevice->setWindowCaption( oexT( "Irrlicht Engine" ) );	
	
		irr::scene::ISceneManager *smgr = m_pDevice->getSceneManager();
		irr::video::IVideoDriver *driver = m_pDevice->getVideoDriver();

	    driver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );

		irr::scene::ICameraSceneNode *camera = 
			smgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ), 
									  irr::core::vector3df( 0, 5, 0 ) );
		
		irr::scene::ISceneNode *node = smgr->addCubeSceneNode( 10.f );
		if ( node )
		{
			irr::scene::ISceneNodeAnimator* rotate = 
				smgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

			node->addAnimator( rotate );

			rotate->drop();

		} // end if

	} // end else

	return 1;
}

int CSqIrrlicht::Draw()
{
	if ( !m_pDevice || !m_pDevice->run() )
		return -1;

	m_pDevice->getVideoDriver()->beginScene( true, true, irr::video::SColor( 255, 100, 101, 140 ) );
	m_pDevice->getSceneManager()->drawAll();
	m_pDevice->getVideoDriver()->endScene();

	return 0;
}

