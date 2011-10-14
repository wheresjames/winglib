
#include <string.h>
#include <jni.h>

#define JFN( f ) Java_winglib_app_alink_CALink_##f

extern "C" jstring JFN(getString)( JNIEnv* env, jobject thiz )
{
    return env->NewStringUTF( "This is alink" );
}
