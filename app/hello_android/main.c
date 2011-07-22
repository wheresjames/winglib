
#include <string.h>
#include <jni.h>

jstring Java_com_example_helloworld_HelloWorld_getString( JNIEnv* env, jobject thiz )
{
    return (*env)->NewStringUTF(env, "Hello World!");
}
