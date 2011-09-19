
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
		return env->NewStringUTF( "irr::getVideoDriver() failed" );

	return env->NewStringUTF( "" );
}

extern "C" jstring Java_winglib_app_irrandroid_IrrRenderer_nativeDraw( JNIEnv* env, jobject thiz )
{
	if ( !g_pDriver )
		return env->NewStringUTF( "g_pDriver is invalid" );

	g_pDriver->beginScene( true, true, irr::video::SColor( 255, 0, 255, 0) );
	g_pDriver->endScene();

	return env->NewStringUTF( "" );
}
