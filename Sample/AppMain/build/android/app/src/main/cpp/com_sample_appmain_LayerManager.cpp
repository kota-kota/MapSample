#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "com_sample_appmain_LayerManager.hpp"
#include "LayerManager.hpp"
#include "Logger.hpp"

using LayerManager = app::LayerManager;

void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1create
        (JNIEnv* env, jobject obj)
{
    LOGI("com_sample_appmain_LayerManager_LayerManager_create");
    LayerManager* p = LayerManager::getInstance();
}

void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1start
        (JNIEnv* env, jobject obj, jobject surface, jint w, jint h)
{
    if (surface != 0) {
        //ウィンドウ作成
        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        LOGI("com_sample_appmain_LayerManager_LayerManager_start native:%p w:%d h:%d", window, w, h);
        LayerManager* p = LayerManager::getInstance();
        p->start((void*)window, w, h);
    }
}

void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1stop
        (JNIEnv* env, jobject obj)
{
    LayerManager* p = LayerManager::getInstance();
    ANativeWindow* window = (ANativeWindow*)p->stop();
    LOGI("com_sample_appmain_LayerManager_LayerManager_stop native:%p", window);
    if(window != nullptr) {
        ANativeWindow_release(window);
    }
}

void JNICALL Java_com_sample_appmain_LayerManager_LayerManager_1destroy
        (JNIEnv* env, jobject obj)
{
    LOGI("com_sample_appmain_LayerManager_LayerManager_destroy");
    LayerManager::delInstance();
}
