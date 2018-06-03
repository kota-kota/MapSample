#include "SampleMain.hpp"
#include "logger.hpp"

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

fw::SampleMain* sample_main = nullptr;
ANativeWindow* window = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow_Jni_nativeCreate(JNIEnv* jenv, jobject obj) {
    LOG_INFO("nativeCreate\n");
    if(sample_main == nullptr) {
        sample_main = new fw::SampleMain();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow_Jni_nativeDestroy(JNIEnv* jenv, jobject obj) {
    LOG_INFO("nativeDestroy\n");
    if(sample_main != nullptr) {
        delete sample_main;
        sample_main = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow_Jni_nativeCreateSurface(JNIEnv* jenv, jobject obj, jobject surface, jint w, jint h) {
    LOG_INFO("nativeCreateSurface\n");
    if(window == nullptr) {
        window = ANativeWindow_fromSurface(jenv, surface);
    }
    if(sample_main != nullptr) {
        sample_main->createSurface(window);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow_Jni_nativeDestroySurface(JNIEnv* jenv, jobject obj) {
    LOG_INFO("nativeDestroySurface\n");
    if(sample_main != nullptr) {
        sample_main->destroySurface();
    }
    if(window != nullptr) {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kyohei_androidwindow_Jni_nativeDraw(JNIEnv* jenv, jobject obj) {
    LOG_INFO("nativeDraw\n");
    if(sample_main != nullptr) {
        sample_main->draw();
    }
}