
#include <string.h>
#include <jni.h>

#define JFN( c, f ) Java_winglib_app_helloworld_##c_##f

jstring JFN( HelloWorld, getString )( JNIEnv* env, jobject thiz )
{
    return (*env)->NewStringUTF(env, "Hello World! - Love C");
}
