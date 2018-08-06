#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1create
        (JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1start
        (JNIEnv* env, jobject obj, jobject surface, jint w, jint h);

JNIEXPORT void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1stop
        (JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1destroy
        (JNIEnv* env, jobject obj);

#ifdef __cplusplus
}
#endif
