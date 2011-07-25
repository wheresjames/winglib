
#include <string.h>
#include <jni.h>

jstring Java_winglib_app_helloworld_HelloWorld_getString( JNIEnv* env, jobject thiz )
{
    return (*env)->NewStringUTF(env, "Hello World! - Love C");
}
