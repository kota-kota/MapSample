#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include "Shader.hpp"

#include <thread>
#include <mutex>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

namespace app {

    class LayerManager {
        //メンバ変数
        bool isTask_;
        bool isPause_;
        std::thread th_;
        std::mutex mtx_;
        void* native_;
        EGLDisplay eglDpy_;
        EGLConfig  eglCfg_;
        EGLContext eglCtx_;
        EGLSurface eglWin_;
        EGLSurface eglPBuf_;
        Shader shader_;

        Int32 w_;
        Int32 h_;

        UInt32 fb_;
        UInt32 cb_;
        UInt32 rb_;

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
        void start(void* native, Int32 w, Int32 h);
        //表示更新停止
        void* stop();
    };
}

#endif //INCLUDED_LAYERMANAGER_HPP
