#include "LayerManager.hpp"
#include "Logger.hpp"
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow2_MainActivity_nativeStart
        (JNIEnv *env, jobject)
{
    //開始
    ui::LayerManager* p = ui::LayerManager::getInstance();
    p->start();
}

JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow2_NativeView_nativeSetSurface
        (JNIEnv *env, jobject, jobject surface)
{
    if (surface != 0) {
        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        LOGI("JniApi::setSurface GET Window (%p)", window);

        //ウィンドウ作成
        ui::LayerManager* p = ui::LayerManager::getInstance();
        p->createWindow(window);
    }
    else {
        ui::LayerManager* p = ui::LayerManager::getInstance();
        ANativeWindow* window = (ANativeWindow*)p->getWindow();

        ANativeWindow_release(window);
        LOGI("JniApi::setSurface RELEASE window (%p)", window);

        //ウィンドウ破棄
        p->destroyWindow();
    }
}

}
