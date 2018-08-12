#include "LayerManager.hpp"
#include "Logger.hpp"

namespace {
    //インスタンスハンドル
    static app::LayerManager* g_instance_ = nullptr;

    //EGLコンフィグ属性
    const EGLint attr_config[] = {
            EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,		EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
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

    //EGLPBuffer属性
    const EGLint attr_pbuffer[] = {
            EGL_WIDTH,  1,
            EGL_HEIGHT, 1,
            EGL_NONE,
    };

    //FBOバーテックスシェーダ
    const GLchar* vertex_fbo =
            "#version 100\n"
            "attribute highp vec3 attr_point;\n"
            "attribute lowp vec2 attr_uv;\n"
            "varying lowp vec2 vary_uv;\n"
            "void main() {\n"
            "    gl_Position = vec4( attr_point, 1.0 );\n"
            "    vary_uv = attr_uv;\n"
            "}\n";

    //FBOフラグメントシェーダ
    const GLchar* fragment_fbo =
            "#version 100\n"
            "uniform lowp sampler2D unif_texture;\n"
            "varying lowp vec2 vary_uv;\n"
            "void main() {\n"
            "    gl_FragColor = texture2D( unif_texture, vary_uv );\n"
            "}\n";
}

namespace app {

    //コンストラクタ
    LayerManager::LayerManager() :
            isTask_(false), isPause_(false), th_(), mtx_(), native_(nullptr),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglCtx_(EGL_NO_CONTEXT), eglWin_(EGL_NO_SURFACE), eglPBuf_(EGL_NO_SURFACE),
            shader_(), w_(0), h_(0), fb_(0), cb_(0), rb_(0)
    {
        LOGI("%s\n", __FUNCTION__);

        //EGLディスプレイを取得
        this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        LOGI("%s eglGetDisplay dpy:%p (err:0x%x)\n", __FUNCTION__, this->eglDpy_, eglGetError());

        //EGL初期化
        EGLint major = 0, minor = 0;
        (void)eglInitialize(this->eglDpy_, &major, &minor);
        LOGI("%s eglInitialize egl:%d.%d (err:0x%x)\n", __FUNCTION__, major, minor, eglGetError());

        //EGLコンフィグを取得
        EGLint numConfigs = 0;
        (void)eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numConfigs);
        LOGI("%s eglChooseConfig cfg:%p num:%d (err:0x%x)\n", __FUNCTION__, this->eglCfg_, numConfigs, eglGetError());

        //EGLコンテキストを作成
        this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
        LOGI("%s eglCreateContext ctx:%p (err:0x%x)\n", __FUNCTION__, this->eglCtx_, eglGetError());

        //EGLPBufferサーフェイスを作成
        this->eglPBuf_ = eglCreatePbufferSurface(this->eglDpy_, this->eglCfg_, attr_pbuffer);
        LOGI("%s eglCreatePbufferSurface pbuf:%p (err:0x%x)\n", __FUNCTION__, this->eglPBuf_, eglGetError());

        //カレント
        (void)eglMakeCurrent(this->eglDpy_, this->eglPBuf_, this->eglPBuf_, this->eglCtx_);
        LOGI("%s eglMakeCurrent bind (err:0x%x)\n", __FUNCTION__, eglGetError());

        //シェーダ作成
        this->shader_.create(vertex_fbo, fragment_fbo);

        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        LOGI("%s eglMakeCurrent unbind (err:0x%x)\n", __FUNCTION__, eglGetError());

        //スレッド作成
        this->th_ = std::thread(&LayerManager::mainTask, this);
    }

    //デストラクタ
    LayerManager::~LayerManager()
    {
        LOGI("%s\n", __FUNCTION__);

        //タスク終了
        this->isTask_ = false;
        this->th_.join();

        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            LOGI("%s eglDestroyContext ctx:%p\n", __FUNCTION__, this->eglCtx_);
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
            this->eglCtx_ = EGL_NO_CONTEXT;
        }
        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            LOGI("%s eglTerminate dpy:%p\n", __FUNCTION__, this->eglDpy_);
            //TODO: eglTerminate(this->eglDpy_);
            this->eglDpy_ = EGL_NO_DISPLAY;
        }
    }

    //メインタスク
    void LayerManager::mainTask()
    {
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
                LOGI("%s eglCreateWindowSurface win:%p (err:0x%x)\n", __FUNCTION__, this->eglWin_, eglGetError());

                //カレント
                (void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
                LOGI("%s eglMakeCurrent bind (err:0x%x)\n", __FUNCTION__, eglGetError());

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
                    LOGI("%s eglMakeCurrent unbind (err:0x%x)\n", __FUNCTION__, eglGetError());
                    if (this->eglWin_ != EGL_NO_SURFACE) {
                        LOGI("%s eglDestroySurface win:%p\n", __FUNCTION__, this->eglWin_);
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

            if(this->fb_ == 0) {
                //カラーバッファ用のテクスチャを用意する
                glGenTextures(1, &this->cb_);
                glBindTexture(GL_TEXTURE_2D, this->cb_);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w_, this->h_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glBindTexture(GL_TEXTURE_2D, 0);

                //デプスバッファ用のレンダーバッファを用意する
                glGenRenderbuffers(1, &this->rb_);
                glBindRenderbuffer(GL_RENDERBUFFER, this->rb_);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->w_, this->h_);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                //フレームバッファオブジェクトを作成する
                glGenFramebuffers(1, &this->fb_);
                glBindFramebuffer(GL_FRAMEBUFFER, this->fb_);

                //フレームバッファオブジェクトにカラーバッファとしてテクスチャを結合する
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->cb_, 0);

                //フレームバッファオブジェクトにデプスバッファとしてレンダーバッファを結合する
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rb_);

                // フレームバッファが完全かどうかチェックします。
                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                LOGI("%s Framebuffer fb:%d cb:%d rb:%d status:0x%x\n", __FUNCTION__, this->fb_, this->cb_, this->rb_, status);

                // フレームバッファオブジェクトの結合を解除する
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            //----------------------------------------------------------------
            // FBOに描画

            //フレームバッファオブジェクトを結合する
            glBindFramebuffer(GL_FRAMEBUFFER, this->fb_);

            //描画
            glClearColor(color / 255.0F, color / 255.0F, color / 255.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            //フレームバッファオブジェクトの結合を解除する
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            //----------------------------------------------------------------
            //FBOを表示面に反映

            GLuint program = this->shader_.getProgramId();
            GLuint attr_point = this->shader_.getAttrLocation("attr_point");
            GLuint attr_uv = this->shader_.getAttrLocation("attr_uv");
            GLuint unif_texture = this->shader_.getUniformLocation("unif_texture");

            //ビューポート設定
            glViewport(0, 0, this->w_, this->h_);

            //使用するシェーダを指定
            glUseProgram(program);

            // テクスチャマッピングを有効にする
            glBindTexture(GL_TEXTURE_2D, this->cb_);
            glEnable(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //テクスチャユニット0を指定
            glUniform1i(unif_texture, 0);

            //頂点データ転送
            GLfloat p[] = {
                    -1.0f, -1.0f,
                     0.0f, -1.0f,
                    -1.0f,  0.0f,
                     0.0f,  0.0f,
            };
            GLfloat uv[] = {
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
            };
            glEnableVertexAttribArray(attr_point);
            glEnableVertexAttribArray(attr_uv);
            glVertexAttribPointer(attr_point, 2, GL_FLOAT, GL_FALSE, 0, &p[0]);
            glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, &uv[0]);

            //描画
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // テクスチャマッピングを無効にする
            glDisableVertexAttribArray(attr_point);
            glDisableVertexAttribArray(attr_uv);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            //表示更新
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
        LOGI("%s eglMakeCurrent unbind (err:0x%x)\n", __FUNCTION__, eglGetError());
        if (this->eglWin_ != EGL_NO_SURFACE) {
            LOGI("%s eglDestroySurface win:%p\n", __FUNCTION__, this->eglWin_);
            eglDestroySurface(this->eglDpy_, this->eglWin_);
            this->eglCtx_ = EGL_NO_SURFACE;
        }
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
    void LayerManager::start(void* native, Int32 w, Int32 h)
    {
        LOGI("%s native:%p\n", __FUNCTION__, native);
        this->mtx_.lock();
        this->native_ = native;
        this->w_ = w;
        this->h_ = h;
        this->mtx_.unlock();
    }

    //表示更新停止
    void* LayerManager::stop()
    {
        LOGI("%s native:%p\n", __FUNCTION__, this->native_);
        this->mtx_.lock();
        void* ret = this->native_;
        this->native_ = nullptr;
        this->w_ = 0;
        this->h_ = 0;
        this->mtx_.unlock();
        return ret;
    }
}