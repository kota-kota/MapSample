#include "LayerManager.hpp"
#include "Logger.hpp"

namespace {
    //インスタンスハンドル
    static ui::LayerManager* g_instance_ = nullptr;

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
            EGL_CONTEXT_CLIENT_VERSION,	3,
            EGL_NONE,
    };
}

namespace ui {

    //コンストラクタ
    LayerManager::LayerManager() :
            isTask_(false), isPause_(false), th_(), mtx_(), native_(nullptr),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT)
    {
        LOGI("%s\n", __FUNCTION__);
        //EGL資源作成
        {
            EGLBoolean retEgl = EGL_FALSE;
            EGLint major = 0, minor = 0, numConfigs = 0;

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
            retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numConfigs);
            if ((retEgl != EGL_TRUE) || (numConfigs != 1)) {
                LOGE("%s eglChooseConfig (num:%d) (err:0x%x)\n", __FUNCTION__, numConfigs, eglGetError());
                goto END;
            }
            LOGI("%s eglChooseConfig (%p) (num:%d)\n", __FUNCTION__, this->eglCfg_, numConfigs);
        }

        //スレッド作成
        {
            this->th_ = std::thread(&LayerManager::mainTask, this);
        }

        END:
        return;
    }

    //デストラクタ
    LayerManager::~LayerManager()
    {
        LOGI("%s\n", __FUNCTION__);
        this->isTask_ = false;
        this->th_.join();

        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            LOGI("%s eglTerminate (%p)\n", __FUNCTION__, this->eglDpy_);
            //TODO: eglTerminate(this->eglDpy_);
            this->eglDpy_ = EGL_NO_DISPLAY;
        }
    }

    //メインタスク
    void LayerManager::mainTask()
    {
        EGLBoolean retEgl = EGL_FALSE;
        GLfloat color = 0.0F;

        bool isPause = true;

        //開始
        LOGI("%s START\n", __FUNCTION__);
        this->isTask_ = true;
        while(true) {
            this->mtx_.lock();
            bool isTask = this->isTask_;
            void* native = this->native_;
            this->mtx_.unlock();

            if(!isTask) {
                //タスク終了
                break;
            }

            //表示資源のチェック
            if(isPause) {
                //表示更新停止中
                if(native == nullptr) {
                    //ウィンドウがない→停止中を継続
                    //LOGI("%s PAUSE\n", __FUNCTION__);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                //ウィンドウがある→生成された

                //資源を生成する
                //EGLウィンドウサーフェイスを作成
                this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)native, nullptr);
                if (this->eglWin_ == EGL_NO_SURFACE) {
                    LOGE("%s eglCreateWindowSurface (err:0x%x)\n", __FUNCTION__, eglGetError());
                    continue;
                }
                LOGI("%s eglCreateWindowSurface (%p)\n", __FUNCTION__, this->eglWin_);

                //EGLコンテキストを作成
                this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
                if (this->eglCtx_ == EGL_NO_CONTEXT) {
                    LOGE("%s eglCreateContext (err:0x%x)\n", __FUNCTION__, eglGetError());
                    continue;
                }
                LOGI("%s eglCreateContext (%p)\n", __FUNCTION__, this->eglCtx_);

                //カレント
                retEgl = eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
                if (retEgl != EGL_TRUE) {
                    LOGE("%s eglMakeCurrent (err:0x%x)\n", __FUNCTION__, eglGetError());
                    continue;
                }
                LOGI("%s eglMakeCurrent\n", __FUNCTION__);

                //表示更新停止を終了し、表示更新実施中にする
                LOGI("%s PAUSE -> RUN\n", __FUNCTION__);
                isPause = false;
            }
            else {
                //表示更新実施中
                if(native == nullptr) {
                    //ウィンドウがない→破棄された

                    //資源を破棄する
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

                    //表示更新を停止する
                    LOGI("%s RUN -> PAUSE\n", __FUNCTION__);
                    isPause = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
                //ウィンドウがある→実施中を継続
                //LOGI("%s RUN\n", __FUNCTION__);
            }

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

        //タスク終了
        LOGI("%s END\n", __FUNCTION__);

        //資源を破棄
        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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

        return;
    }

    //インスタンス取得
    LayerManager* LayerManager::getInstance()
    {
        if(g_instance_ == nullptr) {
            g_instance_ = new LayerManager();
        }
        return g_instance_;
    }

    //インスタンス削除
    void LayerManager::delInstance()
    {
        if(g_instance_ != nullptr) {
            delete g_instance_;
            g_instance_ = nullptr;
        }
    }

    //表示更新開始
    void LayerManager::start(void* native)
    {
        LOGI("%s native:0xp\n", __FUNCTION__, native);
        this->mtx_.lock();
        this->native_ = native;
        this->mtx_.unlock();
    }

    //表示更新停止
    void* LayerManager::stop()
    {
        LOGI("%s native:0x%p\n", __FUNCTION__, this->native_);
        this->mtx_.lock();
        void* ret = this->native_;
        this->native_ = nullptr;
        this->mtx_.unlock();
        return ret;
    }
}