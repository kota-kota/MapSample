#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "com_sample_appmain_AppMain.hpp"
#include "AppMain.hpp"
#include "Logger.hpp"

void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1create
        (JNIEnv* env, jobject obj)
{
    LOGI("com_sample_appmain_AppMainView_AppMain_create");
    app::AppMain::create();
}

void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1destroy
        (JNIEnv* env, jobject obj)
{
    LOGI("com_sample_appmain_AppMainView_AppMain_destroy");
    app::AppMain::destroy();
}

void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1start
        (JNIEnv* env, jobject obj, jobject surface, jint w, jint h)
{
    if (surface != 0) {
        //ウィンドウ作成
        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        LOGI("com_sample_appmain_AppMainView_AppMain_start native:%p w:%d h:%d", window, w, h);
        app::AppMain* p = app::AppMain::get();
        p->start((void*)window, w, h);
    }
}

void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1stop
        (JNIEnv* env, jobject obj)
{
    app::AppMain* p = app::AppMain::get();
    ANativeWindow* window = (ANativeWindow*)p->stop();
    LOGI("com_sample_appmain_AppMainView_AppMain_stop native:%p", window);
    if(window != nullptr) {
        ANativeWindow_release(window);
    }
}

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1ON
        (JNIEnv* env, jobject obj, jfloat x, jfloat y)
{
    LOGI("com_sample_appmain_AppMainView_AppMain_procTouchEvent_ON x:%f y:%f", x, y);
    app::AppMain* p = app::AppMain::get();
    p->onTouchEvent(app::TouchEvent::TOUCH_ON, app::Pos2D<app::Float>(x, y));
}

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1OFF
        (JNIEnv* env, jobject obj, jfloat x, jfloat y)
{
    LOGI("com_sample_appmain_AppMainView_AppMain_procTouchEvent_OFF x:%f y:%f", x, y);
    app::AppMain* p = app::AppMain::get();
    p->onTouchEvent(app::TouchEvent::TOUCH_OFF, app::Pos2D<app::Float>(x, y));
}

JNIEXPORT void JNICALL Java_com_sample_appmain_AppMainView_AppMain_1procTouchEvent_1MOVE
        (JNIEnv* env, jobject obj, jfloat x, jfloat y)
{
    LOGI("com_sample_appmain_AppMainView_AppMain_procTouchEvent_MOVE x:%f y:%f", x, y);
    app::AppMain* p = app::AppMain::get();
    p->onTouchEvent(app::TouchEvent::TOUCH_MOVE, app::Pos2D<app::Float>(x, y));
}
