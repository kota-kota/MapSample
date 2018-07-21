#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include <thread>
#include <mutex>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

namespace ui {

    class LayerManager {
        //メンバ変数
        std::mutex mtx_;
        uint8_t isTask_;
        void* nativeWindow_;
        EGLDisplay eglDpy_;
        EGLConfig  eglCfg_;
        EGLSurface eglWin_;
        EGLContext eglCtx_;

    private:
        //コンストラクタ
        LayerManager();
        //描画タスク
        void renderTask();
        //描画準備
        bool readyRender();
        //描画一時停止
        void pauseRender();

    public:
        //インスタンス取得
        static LayerManager* getInstance();
        //開始
        void start();
        //ウィンドウ作成
        void createWindow(void* nativeWindow);
        //ウィンドウ破棄
        void destroyWindow();
        //ウィンドウ取得
        void* getWindow();
    };
}

#endif //INCLUDED_LAYERMANAGER_HPP
