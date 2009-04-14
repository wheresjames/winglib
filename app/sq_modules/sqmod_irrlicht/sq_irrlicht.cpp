// sq_irrlicht.cpp

#include "stdafx.h"

#define _IRR_COMPILE_WITH_OPENGL_
#if defined( _IRR_COMPILE_WITH_OPENGL_ )
#	include <GL/gl.h>
#endif


CSqIrrlicht::CSqIrrlicht()
{
	m_pDevice = oexNULL;
	m_pSmgr = oexNULL;
	m_pDriver = oexNULL;
	m_pGui = oexNULL;
	m_pCamera = oexNULL;

	m_nDriverType = -1;
	m_colBackground = irr::video::SColor( 255, 0, 50, 100 );

	m_bStereo = 0;
	m_ulREyeKey = 0x0000ffff;
	m_ulLEyeKey = 0x00ff0000;
	m_fStereoWidth = 3.f;
	m_fStereoFocus = 1000.f;
}

CSqIrrlicht::~CSqIrrlicht()
{
	m_pSmgr = oexNULL;
	m_pDriver = oexNULL;
	m_pGui = oexNULL;
	m_pCamera = oexNULL;
	m_nDriverType = -1;

	if ( m_pDevice )
	{	m_pDevice->closeDevice();
		m_pDevice->drop();
		m_pDevice = oexNULL;
	} // end if
}

int CSqIrrlicht::Init( const sqbind::stdString &sName, int width, int height )
{
	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
#if defined( _WIN32_WCE )
	param.DriverType = irr::video::EDT_BURNINGVIDEO;
	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
#else
	param.DriverType = irr::video::EDT_OPENGL;
	param.WindowSize = irr::core::dimension2d<irr::s32>( width, height );
#endif

	m_pDevice = irr::createDeviceEx( param );

	if ( !m_pDevice )
		oexERROR( 0, oexT( "Failed to create irrlicht device" ) );

	else
	{
		// Save away driver type
		m_nDriverType = param.DriverType;

		if ( sName.length() )
			m_pDevice->setWindowCaption( oexStrToStrWPtr( sName.c_str() ) );

		// Grab components
		m_pSmgr = m_pDevice->getSceneManager();
		m_pDriver = m_pDevice->getVideoDriver();
		m_pGui = m_pDevice->getGUIEnvironment();

		if ( !oexCHECK_PTR( m_pSmgr ) )
			oexERROR( 0, oexT( "Scene manager is invalid" ) );

		if ( !oexCHECK_PTR( m_pDriver ) )
			oexERROR( 0, oexT( "Driver pointer is invalid" ) );

		if ( !oexCHECK_PTR( m_pGui ) )
			oexERROR( 0, oexT( "GUI environment is invalid" ) );


	    m_pDriver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
		m_pSmgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
		m_pSmgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );

		m_pCamera =
			m_pSmgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ),
									  irr::core::vector3df( 0, 5, 0 ) );

		irr::scene::ISceneNode *node = m_pSmgr->addCubeSceneNode( 10.f );
		if ( node )
		{
			irr::scene::ISceneNodeAnimator* rotate =
				m_pSmgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

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

    if ( m_bStereo )
		DrawAnaglyph( m_pDriver, m_pSmgr, m_pCamera, m_colBackground,
		              m_fStereoWidth, m_fStereoFocus, m_nDriverType,
					  m_ulREyeKey, m_ulLEyeKey );

	else
	{
		m_pDriver->beginScene( true, true, m_colBackground );
		m_pSmgr->drawAll();
		m_pGui->drawAll();
		m_pDriver->endScene();

	} // end else

	return 0;
}

int CSqIrrlicht::DrawAnaglyph( irr::video::IVideoDriver *pDriver,
                    irr::scene::ISceneManager *pSm,
                    irr::scene::ICameraSceneNode *pCamera,
                    irr::video::SColor colBackground,
                    float fWidth, float fFocus, int nDriverType,
                    unsigned long ulREyeKey, unsigned long ulLEyeKey )
{
	// Right eye
	irr::core::vector3df reye = pCamera->getPosition();

	// Left eye
	irr::core::vector3df v( fWidth, 0, 0 );
	irr::core::matrix4 m;
	m.setRotationDegrees( pCamera->getRotation() + irr::core::vector3df( 90.f, 0, 0 ) );
	m.transformVect( v );
	irr::core::vector3df leye = reye + v;

	// Eye target
	irr::core::vector3df oldt = pCamera->getTarget();
	irr::core::vector3df eyet = ( oldt - reye ).normalize() * fFocus;
	pCamera->setTarget( eyet );

	IDirect3DDevice9 *pDdx9 = NULL;
	if ( nDriverType == irr::video::EDT_DIRECT3D9
			  && pDriver->getExposedVideoData().D3D9.D3DDev9 )
	  pDdx9 = pDriver->getExposedVideoData().D3D9.D3DDev9;

	if ( 0 )
		;

	// Setup right eye
#if defined( _IRR_COMPILE_WITH_OPENGL_ )

	else if ( nDriverType == irr::video::EDT_OPENGL )
	{   glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	  	glColorMask( 0 != ( ulREyeKey & 0x00ff0000 ),
					 0 != ( ulREyeKey & 0x0000ff00 ),
					 0 != ( ulREyeKey & 0x000000ff ),
					 0 != ( ulREyeKey & 0xff000000 ) );
	} // end if

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_	) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
		pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   ( ulREyeKey & 0x00ff0000 ? D3DCOLORWRITEENABLE_RED : 0 )
							   | ( ulREyeKey & 0x0000ff00 ? D3DCOLORWRITEENABLE_GREEN : 0 )
							   | ( ulREyeKey & 0x000000ff ? D3DCOLORWRITEENABLE_BLUE : 0 )
							   | ( ulREyeKey & 0xff000000 ? D3DCOLORWRITEENABLE_ALPHA : 0 ) );
#endif

	pDriver->beginScene( true, true, colBackground );
	pSm->drawAll();

	if ( 0 )
		;

#if defined( _IRR_COMPILE_WITH_OPENGL_ )

	// Left Eye
	if ( nDriverType == irr::video::EDT_OPENGL )
	{   glClear( GL_DEPTH_BUFFER_BIT );
		glMatrixMode( GL_MODELVIEW );
	  	glLoadIdentity();
	  	glColorMask( 0 != ( ulLEyeKey & 0x00ff0000 ),
					 0 != ( ulLEyeKey & 0x0000ff00 ),
					 0 != ( ulLEyeKey & 0x000000ff ),
					 0 != ( ulLEyeKey & 0xff000000 ) );
	} // end if

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_	) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
	{   pDdx9->Clear(0 , 0 , D3DCLEAR_ZBUFFER, 0x000000 , 1.0f , 0);
	  	pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   ( ulLEyeKey & 0x00ff0000 ? D3DCOLORWRITEENABLE_RED : 0 )
							   | ( ulLEyeKey & 0x0000ff00 ? D3DCOLORWRITEENABLE_GREEN : 0 )
							   | ( ulLEyeKey & 0x000000ff ? D3DCOLORWRITEENABLE_BLUE : 0 )
							   | ( ulLEyeKey & 0xff000000 ? D3DCOLORWRITEENABLE_ALPHA : 0 ) );
	} // end else if

#endif

	pCamera->setPosition( leye );
	pCamera->OnRegisterSceneNode();

	pSm->drawAll();
	pDriver->endScene();

	if ( 0 )
		;

#if defined( _IRR_COMPILE_WITH_OPENGL_ )

	if ( nDriverType == irr::video::EDT_OPENGL )
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_	) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
		pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   D3DCOLORWRITEENABLE_BLUE
							   | D3DCOLORWRITEENABLE_GREEN
							   | D3DCOLORWRITEENABLE_RED
							   | D3DCOLORWRITEENABLE_ALPHA );
#endif

	// Restore original position
	pCamera->setPosition( reye );
	pCamera->setTarget( oldt );

	return 1;
}

int CSqIrrlicht::AddSkyDome( const sqbind::stdString &sFile,
							 long lHorzRes, long lVertRes,
							 float fTxtPercent, float fSpherePercent )
{
	if ( !sFile.length() )
		return 0;

    irr::scene::ISceneNode *pNode =
        m_pSmgr->addSkyDomeSceneNode( m_pDriver->getTexture( sFile.c_str() ),
                                      lHorzRes, lVertRes, fTxtPercent, fSpherePercent );

    return 1;
}
