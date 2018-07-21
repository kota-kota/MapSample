#include "LayerManager.hpp"
#include "Logger.hpp"

namespace {
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
}

namespace ui {

    //コンストラクタ
    LayerManager::LayerManager() :
            mtx_(), nativeWindow_(nullptr), isTask_(0),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT)
    {
    }

    //描画タスク
    void LayerManager::renderTask()
    {
        bool isRenderResource = false;
        EGLBoolean retEgl = EGL_FALSE;
        EGLint major = 0, minor = 0, numCfgs = 0;

        GLfloat color = 0.0F;

        //EGLディスプレイを取得
        this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (this->eglDpy_ == EGL_NO_DISPLAY) {
            LOGE("%s eglGetDisplay (err:0x%x)\n", __FUNCTION__, eglGetError());
            goto END;
        }
        LOGI("%s eglGetDisplay (%p)\n", __FUNCTION__, this->eglDpy_);

        //EGL初期化
        retEgl = eglInitialize(this->eglDpy_, &major, &minor);
        if (retEgl != EGL_TRUE) {
            LOGE("%s eglInitialize (err:0x%x)\n", __FUNCTION__, eglGetError());
            goto END;
        }
        LOGI("%s eglInitialize (EGL%d.%d)\n", __FUNCTION__, major, minor);

        //EGLコンフィグを取得
        retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numCfgs);
        if ((retEgl != EGL_TRUE) || (numCfgs != 1)) {
            LOGE("%s eglChooseConfig (num:%d) (err:0x%x)\n", __FUNCTION__, numCfgs, eglGetError());
            goto END;
        }
        LOGI("%s eglChooseConfig (%p) (num:%d)\n", __FUNCTION__, this->eglCfg_, numCfgs);

        //開始
        while(true) {
            this->mtx_.lock();
            uint8_t isTask = this->isTask_;
            void* nativeWindow = this->nativeWindow_;
            this->mtx_.unlock();

            if(isTask == 0) {
                //タスク終了
                break;
            }

            if(nativeWindow == nullptr) {
                LOGI("%s PAUSE\n", __FUNCTION__);

                if(isRenderResource) {
                    //描画資源が作成されているため破棄する(描画一時停止)
                    this->pauseRender();
                    isRenderResource = false;
                }

                //100ms待ち
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if(!isRenderResource) {
                //描画資源が作成されていないため作成する(描画準備)
                isRenderResource = this->readyRender();
                if(!isRenderResource) {
                    //作成失敗
                    LOGE("%s readyRender\n", __FUNCTION__);
                    isTask = 0;
                }
            }

            //LOGI("%s RENDER\n", __FUNCTION__);
            glClearColor(color / 255.0F, color / 255.0F, color / 255.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            eglSwapBuffers(this->eglDpy_, this->eglWin_);

            color += 1.0F;
            if(color > 255.0F) {
                color = 0.0F;
            }

            //待ち
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        END:
        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            LOGI("%s eglDestroyContext %p\n", __FUNCTION__, this->eglCtx_);
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
            this->eglCtx_ = EGL_NO_CONTEXT;
        }
        if (this->eglWin_ != EGL_NO_SURFACE) {
            LOGI("%s eglDestroySurface %p\n", __FUNCTION__, this->eglWin_);
            eglDestroySurface(this->eglDpy_, this->eglWin_);
            this->eglCtx_ = EGL_NO_SURFACE;
        }
        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            LOGI("%s eglTerminate (%p)\n", __FUNCTION__, this->eglDpy_);
            //TODO: eglTerminate(this->eglDpy_);
            this->eglDpy_ = EGL_NO_DISPLAY;
        }
        return;
    }

    //描画準備
    bool LayerManager::readyRender()
    {
        bool ret = false;
        EGLBoolean retEgl;

        //EGLウィンドウサーフェイスを作成
        this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)this->nativeWindow_, nullptr);
        if (this->eglWin_ == EGL_NO_SURFACE) {
            LOGE("%s eglCreateWindowSurface (err:0x%x)\n", __FUNCTION__, eglGetError());
            goto END;
        }
        LOGI("%s eglCreateWindowSurface (%p)\n", __FUNCTION__, this->eglWin_);

        //EGLコンテキストを作成
        this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
        if (this->eglCtx_ == EGL_NO_CONTEXT) {
            LOGE("%s eglCreateContext (err:0x%x)\n", __FUNCTION__, eglGetError());
            goto END;
        }
        LOGI("%s eglCreateContext (%p)\n", __FUNCTION__, this->eglCtx_);

        //カレント
        retEgl = eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
        if (retEgl != EGL_TRUE) {
            LOGE("%s eglMakeCurrent (err:0x%x)\n", __FUNCTION__, eglGetError());
            goto END;
        }
        LOGI("%s eglMakeCurrent\n", __FUNCTION__);

        ret = true;

        END:
        return ret;
    }

    //描画一時停止
    void LayerManager::pauseRender()
    {
        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            LOGI("%s eglDestroyContext %p\n", __FUNCTION__, this->eglCtx_);
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
            this->eglCtx_ = EGL_NO_CONTEXT;
        }
        if (this->eglWin_ != EGL_NO_SURFACE) {
            LOGI("%s eglDestroySurface %p\n", __FUNCTION__, this->eglWin_);
            eglDestroySurface(this->eglDpy_, this->eglWin_);
            this->eglWin_ = EGL_NO_SURFACE;
        }
    }

    //インスタンス取得
    LayerManager* LayerManager::getInstance()
    {
        static LayerManager instance_;
        return &instance_;
    }

    //開始
    void LayerManager::start()
    {
        LOGI("%s\n", __FUNCTION__);

        this->mtx_.lock();
        uint8_t isTask = this->isTask_;
        this->mtx_.unlock();

        if(isTask == 0) {
            this->mtx_.lock();
            this->isTask_ = 1;
            this->mtx_.unlock();

            //描画タスク作成
            std::thread th(&LayerManager::renderTask, this);
            th.detach();
        }
    }

    //ウィンドウ作成
    void LayerManager::createWindow(void* nativeWindow)
    {
        LOGI("%s (%p)\n", __FUNCTION__, nativeWindow);

        this->mtx_.lock();
        this->nativeWindow_ = nativeWindow;
        this->mtx_.unlock();
    }

    //ウィンドウ破棄
    void LayerManager::destroyWindow()
    {
        LOGI("%s (%p)\n", __FUNCTION__, this->nativeWindow_);

        this->mtx_.lock();
        this->nativeWindow_ = nullptr;
        this->mtx_.unlock();
    }

    //ウィンドウ取得
    void* LayerManager::getWindow()
    {
        return this->nativeWindow_;
    }
}