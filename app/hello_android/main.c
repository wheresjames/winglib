
// http://android-developers.blogspot.com/2009/04/introducing-glsurfaceview.html
// http://www.codexperiments.com/android/2010/05/android-introduction-part-2-setting-up-your-environment/
// http://forum.xda-developers.com/showthread.php?s=811d4d53febd3a2e2ea14cbd12c8f31c&t=618201&page=2

#include <string.h>
#include <jni.h>

#define JFN( c, f ) Java_winglib_app_helloworld_##c_##f

jstring JFN( HelloWorld, getString )( JNIEnv* env, jobject thiz )
{
    return (*env)->NewStringUTF(env, "Hello World! - Love C");
}
