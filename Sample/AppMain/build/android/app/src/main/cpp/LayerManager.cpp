#include "LayerManager.hpp"
#include "Logger.hpp"

namespace {
    //インスタンスハンドル
    static app::LayerManager* g_instance_ = nullptr;

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

    //------------------------------------------------------------------------------------
    // Layer

    //コンストラクタ
    Layer::Layer() :
            x_(0), y_(0), w_(0), h_(0), fb_(0), cb_(0), rb_(0)
    {
        LOGI("%s\n", __FUNCTION__);
    }

    //デストラクタ
    Layer::~Layer()
    {
        LOGI("%s\n", __FUNCTION__);
        this->destroy();
    }

    //レイヤーID取得
    UInt32 Layer::getLayerID()
    {
        return this->cb_;
    }

    //レイヤー作成チェック
    bool Layer::isCreated()
    {
        bool ret = false;
        if (this->fb_ != 0) { ret = true; }
        return ret;
    }

    //レイヤー作成
    ReturnCode Layer::create(Int32 w, Int32 h)
    {
        LOGI("%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;

        this->w_ = w;
        this->h_ = h;

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

        //フレームバッファが正常に作成できたかチェックする
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        //フレームバッファオブジェクトの結合を解除する
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        LOGI("%s fb:%d cb:%d rb:%d status:0x%x\n", __FUNCTION__, this->fb_, this->cb_, this->rb_, status);
        if(status != GL_FRAMEBUFFER_COMPLETE) { goto END; }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //レイヤー破棄
    void Layer::destroy()
    {
        LOGI("%s fb:%d cb:%d rb:%d\n", __FUNCTION__, this->fb_, this->cb_, this->rb_);
        glDeleteTextures(1, &this->cb_);
        glDeleteRenderbuffers(1, &this->rb_);
        glDeleteFramebuffers(1, &this->fb_);
    }

    //レイヤーに対する描画開始
    void Layer::beginDraw()
    {
        //フレームバッファオブジェクトを結合する
        glBindFramebuffer(GL_FRAMEBUFFER, this->fb_);
    }

    //レイヤーに対する描画終了
    void Layer::endDraw()
    {
        //フレームバッファオブジェクトの結合を解除する
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //レイヤー画面位置移動
    void Layer::movePosition(Int32 x, Int32 y)
    {

    }



    //------------------------------------------------------------------------------------
    // LayerManager

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

    //コンストラクタ
    LayerManager::LayerManager() :
            isTask_(false), isPause_(false), th_(), mtx_(), w_(0), h_(0), native_(nullptr),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglCtx_(EGL_NO_CONTEXT), eglWin_(EGL_NO_SURFACE),
            shader_(), layerNum_(0), layerList_()
    {
        LOGI("%s\n", __FUNCTION__);

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
    }

    //メインタスク
    void LayerManager::mainTask()
    {
        LOGI("%s START\n", __FUNCTION__);
        ReturnCode retCode;
        GLfloat color = 0.0F;

        //EGL資源の作成
        retCode = this->createEGL();
        if(retCode != OK) {
            goto END;
        }

        //タスクループ開始
        this->isTask_ = true;
        this->isPause_ = true;
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
            if(this->isPause_) {
                //表示更新停止中
                if(native == nullptr) {
                    //ウィンドウがない→停止中を継続
                    //LOGI("%s PAUSE\n", __FUNCTION__);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                //ウィンドウがある→生成された

                //EGLウィンドウサーフェイスを作成する
                retCode = this->createWindowSurface(native);
                if(retCode != OK) {
                    break;
                }

                //表示更新停止を終了し、表示更新実施中にする
                LOGI("%s PAUSE -> RUN\n", __FUNCTION__);
                this->isPause_ = false;
            }
            else {
                //表示更新実施中
                if(native == nullptr) {
                    //ウィンドウがない→破棄された

                    //EGLウィンドウサーフェイスを破棄する
                    this->destroyWindowSurface();

                    //表示更新を停止する
                    LOGI("%s RUN -> PAUSE\n", __FUNCTION__);
                    this->isPause_ = true;
                    continue;
                }
                //ウィンドウがある→実施中を継続
                //LOGI("%s RUN\n", __FUNCTION__);
            }

            if(!this->shader_.isCreated()) {
                //シェーダ作成
                this->shader_.create(vertex_fbo, fragment_fbo);
            }

            for(Int32 iLayer = 0; iLayer < this->maxLayerNum_; ++iLayer) {
                Layer& layer = this->layerList_[iLayer];
                if (!layer.isCreated()) {
                    //描画レイヤー作成
                    layer.create(this->w_, this->h_);
                    this->layerNum_++;
                }

                //FBOに描画
                layer.beginDraw();
                glClearColor(color / 255.0F, color / 255.0F, color / 255.0F, 1.0F);
                glClear(GL_COLOR_BUFFER_BIT);
                glFlush();
                layer.endDraw();
            }

            //レイヤー表示更新
            this->updateLayers();

            color += 1.0F;
            if(color > 255.0F) {
                color = 0.0F;
            }

            //待ち
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("%s !!!ERROR!!!\n", __FUNCTION__);
        }
        //EGL資源の解放
        this->destroyWindowSurface();
        this->destroyEGL();

        //タスク終了
        LOGI("%s END (retCode:%d)\n", __FUNCTION__, retCode);
    }

    //EGL資源作成
    ReturnCode LayerManager::createEGL()
    {
        LOGI("%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;
        EGLint major = 0, minor = 0;
        EGLint numConfigs = 0;

        //EGLディスプレイを取得
        this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        LOGI("%s eglGetDisplay dpy:%p (err:0x%x)\n", __FUNCTION__, this->eglDpy_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGL初期化
        (void)eglInitialize(this->eglDpy_, &major, &minor);
        LOGI("%s eglInitialize egl:%d.%d (err:0x%x)\n", __FUNCTION__, major, minor, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGLコンフィグを取得
        (void)eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numConfigs);
        LOGI("%s eglChooseConfig cfg:%p num:%d (err:0x%x)\n", __FUNCTION__, this->eglCfg_, numConfigs, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGLコンテキストを作成
        this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
        LOGI("%s eglCreateContext ctx:%p (err:0x%x)\n", __FUNCTION__, this->eglCtx_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //EGL資源解放
    void LayerManager::destroyEGL()
    {
        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
            LOGI("%s eglDestroyContext ctx:%p (err:0x%x)\n", __FUNCTION__, this->eglCtx_, eglGetError());
            this->eglCtx_ = EGL_NO_CONTEXT;
        }
        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            //TODO: eglTerminate(this->eglDpy_);
            LOGI("%s eglTerminate dpy:%p (err:0x%x)\n", __FUNCTION__, this->eglDpy_, eglGetError());
            this->eglDpy_ = EGL_NO_DISPLAY;
        }
    }

    //EGLウィンドウサーフェイス作成
    ReturnCode LayerManager::createWindowSurface(void* native)
    {
        ReturnCode retCode = NG_ERROR;

        this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)native, nullptr);
        LOGI("%s eglCreateWindowSurface win:%p (err:0x%x)\n", __FUNCTION__, this->eglWin_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //カレント
        (void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
        LOGI("%s eglMakeCurrent bind (err:0x%x)\n", __FUNCTION__, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //EGLウィンドウサーフェイス解放
    void LayerManager::destroyWindowSurface()
    {
        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        LOGI("%s eglMakeCurrent unbind (err:0x%x)\n", __FUNCTION__, eglGetError());

        if (this->eglWin_ != EGL_NO_SURFACE) {
            eglDestroySurface(this->eglDpy_, this->eglWin_);
            LOGI("%s eglDestroySurface win:%p (err:0x%x)\n", __FUNCTION__, this->eglWin_, eglGetError());
            this->eglWin_ = EGL_NO_SURFACE;
        }
    }

    //レイヤー表示更新
    void LayerManager::updateLayers()
    {
        GLuint program = this->shader_.getProgramId();
        GLuint attr_point = this->shader_.getAttrLocation("attr_point");
        GLuint attr_uv = this->shader_.getAttrLocation("attr_uv");
        GLuint unif_texture = this->shader_.getUniformLocation("unif_texture");

        //ビューポート設定
        glViewport(0, 0, this->w_, this->h_);

        //使用するシェーダを指定
        glUseProgram(program);

        //テクスチャマッピングを有効にする
        glEnable(GL_TEXTURE_2D);
        glEnableVertexAttribArray(attr_point);
        glEnableVertexAttribArray(attr_uv);

        for(Int32 iLayer = 0; iLayer < this->layerNum_; ++iLayer) {
            Layer& layer = this->layerList_[iLayer];

            glBindTexture(GL_TEXTURE_2D, layer.getLayerID());

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
                    -1.0f, 0.0f,
                    0.0f, 0.0f,
            };
            GLfloat uv[] = {
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
            };
            glVertexAttribPointer(attr_point, 2, GL_FLOAT, GL_FALSE, 0, &p[0]);
            glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, &uv[0]);

            //描画
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //テクスチャマッピングを無効にする
        glDisableVertexAttribArray(attr_point);
        glDisableVertexAttribArray(attr_uv);
        glDisable(GL_TEXTURE_2D);

        //表示更新
        (void)eglSwapBuffers(this->eglDpy_, this->eglWin_);
    }
}