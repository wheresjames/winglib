
#include <string.h>
#include <jni.h>

#include "irrlicht.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include "importgl.h"
#define _MSG( s ) __android_log_print( ANDROID_LOG_INFO, OEX_PROJECT_NAME, "%s(%lu): %s\n", __FILE__, __LINE__, s ), \
				  printf( "%s(%lu): %s\n", __FILE__, __LINE__, s )
  
irr::IrrlichtDevice 		*g_pDevice = 0;
irr::video::IVideoDriver	*g_pDriver = 0;
irr::scene::ISceneManager 	*g_pSmgr = 0;
irr::gui::IGUIEnvironment	*g_pGuienv = 0;
irr::gui::IGUIStaticText 	*g_pText = 0;
int							g_frames = 0;

extern "C" jstring Java_winglib_app_irrandroid_IrrRenderer_nativeInit( JNIEnv* env, jobject thiz )
{
    importGLInit();

	_MSG( "Using: EDT_OGLES2" );
    g_pDevice = irr::createDevice( irr::video::EDT_OGLES2, 
								   irr::core::dimension2d< irr::u32 >( 320, 480 ), 
								   16, false, false, false, 0 );
								   
	_MSG( "irr::createDevice() completed" );
	
	if ( !g_pDevice )
		return env->NewStringUTF( "irr::createDevice() failed" );

	g_pDriver = g_pDevice->getVideoDriver();
	if ( !g_pDriver )
		return env->NewStringUTF( "getVideoDriver() failed" );
		
	g_pSmgr = g_pDevice->getSceneManager();
	if ( !g_pSmgr )
		return env->NewStringUTF( "getSceneManager() failed" );

	g_pGuienv = g_pDevice->getGUIEnvironment();
	if ( !g_pSmgr )
		return env->NewStringUTF( "getGUIEnvironment() failed" );
		
	g_pDevice->setWindowCaption( L"Irrlicht Demo" );
	
	g_pText = g_pGuienv->addStaticText( L"FPS: xx", irr::core::rect< irr::s32 >( 60, 5, 200, 20 ), false );
	g_pGuienv->addButton( irr::core::rect< int >( 10, 5, 50, 20), 0, 2, L"Quit" );

	g_pDriver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
	g_pSmgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
								irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
	g_pSmgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
								irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );

//	irr::scene::ICameraSceneNode *camera =
		g_pSmgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ),
									 irr::core::vector3df( 0, 5, 0 ) );

	irr::scene::ISceneNode *node = g_pSmgr->addCubeSceneNode( 10.f );
	if ( node )
	{
		irr::scene::ISceneNodeAnimator* rotate =
			g_pSmgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

		node->addAnimator( rotate );

		rotate->drop();

	} // end if

	return env->NewStringUTF( "" );
}

extern "C" jstring Java_winglib_app_irrandroid_IrrRenderer_nativeDraw( JNIEnv* env, jobject thiz )
{
	if ( !g_pDevice )
		return env->NewStringUTF( "g_pDevice is invalid" );

	g_pDevice->run();
	//if ( !g_pDevice->run() )
	//	return env->NewStringUTF( "g_pDevice->run() returned false" );

	if ( ++g_frames == 10 )
	{
		irr::core::stringw str = L"FPS: ";
		str += (int)g_pDevice->getVideoDriver()->getFPS();

		if ( g_pText )
			g_pText->setText ( str.c_str() );

		g_frames = 0;

	} // end if

	g_pDevice->getVideoDriver()->beginScene( true, true, irr::video::SColor( 255, 100, 101, 140 ) );
	g_pDevice->getSceneManager()->drawAll();
	g_pDevice->getGUIEnvironment()->drawAll();
	g_pDevice->getVideoDriver()->endScene();

	return env->NewStringUTF( "" );
}
