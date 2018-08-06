#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include <thread>
#include <mutex>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

namespace ui {

    class LayerManager {
        //メンバ変数
        bool isTask_;
        bool isPause_;
        std::thread th_;
        std::mutex mtx_;
        void* native_;
        EGLDisplay eglDpy_;
        EGLConfig  eglCfg_;
        EGLSurface eglWin_;
        EGLContext eglCtx_;

    private:
        //コンストラクタ
        LayerManager();
        //デストラクタ
        ~LayerManager();
        //メインタスク
        void mainTask();

    public:
        //インスタンス取得
        static LayerManager* getInstance();
        //インスタンス削除
        static void delInstance();
        //表示更新開始
        void start(void* native);
        //表示更新停止
        void* stop();
    };
}

#endif //INCLUDED_LAYERMANAGER_HPP
