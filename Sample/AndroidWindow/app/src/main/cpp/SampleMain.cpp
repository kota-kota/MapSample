#include "SampleMain.hpp"
#include "logger.hpp"

namespace fw {
    static bool swapdraw = false;

    //コンストラクタ
    SampleMain::SampleMain() :
        nativeWindow_(nullptr), eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT)
    {
    }

    //デストラクタ
    SampleMain::~SampleMain()
    {
    }

    //サーフェイス作成
    bool SampleMain::createSurface(void* nativeWindow)
    {
        bool ret = false;
        EGLBoolean retEgl = EGL_FALSE;
        EGLint numCfgs = 0;

        this->nativeWindow_ = nativeWindow;

        //EGLコンフィグ属性
        const EGLint attr_config[] = {
                EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
                EGL_RED_SIZE,			8,
                EGL_GREEN_SIZE,		8,
                EGL_BLUE_SIZE,			8,
                EGL_ALPHA_SIZE,		8,
                EGL_DEPTH_SIZE,		8,
                EGL_NONE,
        };

        //EGLコンテキスト属性
        const EGLint attr_context[] = {
                EGL_CONTEXT_CLIENT_VERSION,	2,
                EGL_NONE,
        };

        //EGLディスプレイを取得
        this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (this->eglDpy_ == EGL_NO_DISPLAY) {
            //失敗
            LOG_INFO("[ERROR] %s:%d eglGetDisplay\n", __FUNCTION__, __LINE__);
            goto END;
        }
        LOG_INFO("eglGetDisplay %p\n", this->eglDpy_);

        //EGL初期化
        EGLint major, minor;
        retEgl = eglInitialize(this->eglDpy_, &major, &minor);
        if (retEgl != EGL_TRUE) {
            //失敗
            LOG_INFO("[ERROR] %s:%d eglInitialize\n", __FUNCTION__, __LINE__);
            goto END;
        }
        LOG_INFO("eglInitialize EGL%d.%d\n", major, minor);

        //EGLコンフィグを取得
        retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numCfgs);
        if ((retEgl != EGL_TRUE) || (numCfgs != 1)) {
            //失敗
            LOG_INFO("[ERROR] %s:%d eglChooseConfig %d\n", __FUNCTION__, __LINE__, numCfgs);
            goto END;
        }
        LOG_INFO("eglChooseConfig %p %d\n", this->eglCfg_, numCfgs);

        //EGLウィンドウサーフェイスを作成
        this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)this->nativeWindow_, nullptr);
        if (this->eglWin_ == EGL_NO_SURFACE) {
            //失敗
            LOG_INFO("[ERROR] %s:%d eglCreateWindowSurface\n", __FUNCTION__, __LINE__);
            goto END;
        }
        LOG_INFO("eglCreateWindowSurface %p\n", this->eglWin_);

        //EGLコンテキストを作成
        this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
        if (this->eglCtx_ == EGL_NO_CONTEXT) {
            //失敗
            LOG_INFO("[ERROR] %s:%d eglCreateContext\n", __FUNCTION__, __LINE__);
            goto END;
        }
        LOG_INFO("eglCreateContext %p\n", this->eglCtx_);

        //正常終了
        ret = true;

        END:
        return ret;
    }

    //サーフェイス破棄
    void SampleMain::destroySurface()
    {
        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            LOG_INFO("eglDestroyContext %p\n", this->eglCtx_);
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
        }
        if (this->eglWin_ != EGL_NO_SURFACE) {
            LOG_INFO("eglDestroySurface %p\n", this->eglWin_);
            eglDestroySurface(this->eglDpy_, this->eglWin_);
        }
        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            LOG_INFO("eglTerminate %p\n", this->eglDpy_);
            eglTerminate(this->eglDpy_);
        }
        this->nativeWindow_ = nullptr;
    }

    //描画
    void SampleMain::draw()
    {
        LOG_INFO("SampleMain::draw\n");

        (void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);

        glClear(GL_COLOR_BUFFER_BIT);
        if(swapdraw) {
            glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
            swapdraw = false;
        }
        else {
            glClearColor(0.0F, 1.0F, 0.0F, 1.0F);
            swapdraw = true;
        }

        (void)eglSwapBuffers(this->eglDpy_, this->eglWin_);
        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
}
