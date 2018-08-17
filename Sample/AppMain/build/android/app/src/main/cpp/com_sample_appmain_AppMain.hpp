#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1create
        (JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1destroy
        (JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1start
        (JNIEnv* env, jobject obj, jobject surface, jint w, jint h);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1stop
        (JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1ON
        (JNIEnv* env, jobject obj, jfloat x, jfloat y);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1OFF
        (JNIEnv* env, jobject obj, jfloat x, jfloat y);

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1MOVE
        (JNIEnv* env, jobject obj, jfloat x, jfloat y);

#ifdef __cplusplus
}
#endif
