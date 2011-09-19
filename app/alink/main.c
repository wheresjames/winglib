
#include <string.h>
#include <jni.h>

#define JFN( c, f ) Java_winglib_app_alink_##c_##f

jstring JFN( CALink, getString )( JNIEnv* env, jobject thiz )
{
    return (*env)->NewStringUTF( env, "This is alink" );
}
