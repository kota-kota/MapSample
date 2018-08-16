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
            "uniform highp mat4 unif_mvp;\n"
            "varying lowp vec2 vary_uv;\n"
            "void main() {\n"
            "    gl_Position = unif_mvp * vec4( attr_point, 1.0 );\n"
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
            pos_(), size_(), fbo_(), vbo_()
    {
        LOGI("%s\n", __FUNCTION__);
        for(Int32 i = 0; i < FboType::FBO_MAX; i++) {
            this->fbo_[i] = 0U;
        }
        for(Int32 i = 0; i < VboType::VBO_MAX; i++) {
            this->vbo_[i] = 0U;
        }
    }

    //デストラクタ
    Layer::~Layer()
    {
        LOGI("%s\n", __FUNCTION__);
        this->destroy();
    }

    //テクスチャID取得
    UInt32 Layer::getTextureID()
    {
        return this->fbo_[FboType::FBO_COLOR];
    }

    //座標頂点データID取得
    UInt32 Layer::getCoordVetexID()
    {
        return this->vbo_[VboType::VBO_COORD];
    }

    //テクスチャ座標頂点データID取得
    UInt32 Layer::getTexCoordVetexID()
    {
        return this->vbo_[VboType::VBO_TEXCOORD];
    }

    //レイヤー作成チェック
    bool Layer::isCreated()
    {
        bool ret = false;
        if (this->fbo_[FboType::FBO] != 0) { ret = true; }
        return ret;
    }

    //レイヤー作成
    ReturnCode Layer::create(Pos2D<Int32> pos, Size<Int32> size)
    {
        LOGI("%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;

        this->pos_ = pos;
        this->size_ = size;

        //描画エリアを計算
        const Area drawArea = this->calcDrawArea();

        //座標
        Float coords[] = {
                static_cast<Float>(drawArea.xmin), static_cast<Float>(drawArea.ymin),
                static_cast<Float>(drawArea.xmax), static_cast<Float>(drawArea.ymin),
                static_cast<Float>(drawArea.xmin), static_cast<Float>(drawArea.ymax),
                static_cast<Float>(drawArea.xmax), static_cast<Float>(drawArea.ymax),
        };
        const Int32 szCoords = 4 * 2 * sizeof(Float);

        //テクスチャ座標
        Float texcoords[] = {
                0.0F, 0.0F,
                1.0F, 0.0F,
                0.0F, 1.0F,
                1.0F, 1.0F,
        };
        const Int32 szTexCoords = 4 * 2 * sizeof(Float);

        //カラーバッファ用のテクスチャを用意する
        glGenTextures(1, &this->fbo_[FboType::FBO_COLOR]);
        glBindTexture(GL_TEXTURE_2D, this->fbo_[FboType::FBO_COLOR]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->size_.getWidth(), this->size_.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //デプスバッファ用のレンダーバッファを用意する
        glGenRenderbuffers(1, &this->fbo_[FboType::FBO_DEPTH]);
        glBindRenderbuffer(GL_RENDERBUFFER, this->fbo_[FboType::FBO_DEPTH]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->size_.getWidth(), this->size_.getHeight());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        //フレームバッファオブジェクトを作成する
        glGenFramebuffers(1, &this->fbo_[FboType::FBO]);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo_[FboType::FBO]);

        //フレームバッファオブジェクトにカラーバッファとしてテクスチャを結合する
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->fbo_[FboType::FBO_COLOR], 0);

        //フレームバッファオブジェクトにデプスバッファとしてレンダーバッファを結合する
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->fbo_[FboType::FBO_DEPTH]);

        //フレームバッファが正常に作成できたかチェックする
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        //フレームバッファオブジェクトの結合を解除する
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        LOGI("%s fbo:%d color:%d depth:%d status:0x%x\n", __FUNCTION__, this->fbo_[FboType::FBO], this->fbo_[FboType::FBO_COLOR], this->fbo_[FboType::FBO_DEPTH], status);
        if(status != GL_FRAMEBUFFER_COMPLETE) { goto END; }

        //バーテックスバッファオブジェクトを作成する
        glGenBuffers(VboType::VBO_MAX, &this->vbo_[0]);

        //座標格納用バーテックスバッファオブジェクトをGL_ARRAY_BUFFERでバインド
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_[VboType::VBO_COORD]);
        //座標を転送
        glBufferData(GL_ARRAY_BUFFER, szCoords, coords, GL_STATIC_DRAW);

        //テクスチャ座標格納用バーテックスバッファオブジェクトをGL_ARRAY_BUFFERでバインド
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_[VboType::VBO_TEXCOORD]);
        //テクスチャ座標を転送
        glBufferData(GL_ARRAY_BUFFER, szTexCoords, texcoords, GL_STATIC_DRAW);

        //GL_ARRAY_BUFFERでのバインドを解除
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        LOGI("%s vbo_coord:%d vbo_texcoord:%d\n", __FUNCTION__, this->vbo_[VboType::VBO_COORD], this->vbo_[VboType::VBO_TEXCOORD]);

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
        LOGI("%s fbo:%d color:%d depth:%d\n", __FUNCTION__, this->fbo_[FboType::FBO], this->fbo_[FboType::FBO_COLOR], this->fbo_[FboType::FBO_DEPTH]);
        glDeleteTextures(1, &this->fbo_[FboType::FBO_COLOR]);
        glDeleteRenderbuffers(1, &this->fbo_[FboType::FBO_DEPTH]);
        glDeleteFramebuffers(1, &this->fbo_[FboType::FBO]);
        LOGI("%s vbo_coord:%d vbo_texcoord:%d\n", __FUNCTION__, this->vbo_[VboType::VBO_COORD], this->vbo_[VboType::VBO_TEXCOORD]);
        glDeleteBuffers(VboType::VBO_MAX, &this->vbo_[0]);
    }

    //レイヤーに対する描画開始
    void Layer::beginDraw()
    {
        //フレームバッファオブジェクトを結合する
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo_[FboType::FBO]);
    }

    //レイヤーに対する描画終了
    void Layer::endDraw()
    {
        //フレームバッファオブジェクトの結合を解除する
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //画面上位置取得
    Pos2D<Int32> Layer::getPos() const
    {
        return this->pos_;
    }

    //画面上位置更新
    void Layer::updatePos(const Pos2D<Int32> pos)
    {
        this->pos_ = pos;
    }

    //描画エリアを計算
    Area Layer::calcDrawArea() const
    {
        Area area;
        area.xmin = this->pos_.getX();
        area.ymin = this->pos_.getY();
        area.xmax = area.xmin + this->size_.getWidth();
        area.ymax = area.ymin + this->size_.getHeight();
        return area;
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
        LOGI("%s native:%p w:%d h:%d\n", __FUNCTION__, native, w, h);

        this->mtx_.lock();

        //表示資源の設定
        this->native_ = native;
        this->windowSize_.setWidth(w);
        this->windowSize_.setHeight(h);

        //画面幅高さに対応した正射影行列を生成する
        this->projMat_.orthogonal(0.0F, static_cast<Float>(w), static_cast<Float>(h), 0.0F, -1.0F, 1.0F);
        this->projMat_.transpose();

        this->mtx_.unlock();
    }

    //表示更新停止
    void* LayerManager::stop()
    {
        LOGI("%s native:%p\n", __FUNCTION__, this->native_);

        this->mtx_.lock();

        //表示資源のリセット
        void* ret = this->native_;
        this->native_ = nullptr;
        this->windowSize_.setWidth(0);
        this->windowSize_.setHeight(0);

        this->mtx_.unlock();
        return ret;
    }

    //コンストラクタ
    LayerManager::LayerManager() :
            isTask_(false), isPause_(false), th_(), mtx_(), windowSize_(), native_(nullptr), projMat_(),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglCtx_(EGL_NO_CONTEXT), eglWin_(EGL_NO_SURFACE),
            shader_(), layerNum_(0), layerList_()
    {
        LOGI("%s\n", __FUNCTION__);

        //Mathクラスのテスト関数
        app::test_Math();

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
        GLfloat red = 0.0F;
        GLfloat blue = 0.0F;

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

            Layer* layer;
            layer = &this->layerList_[0];
            if (!layer->isCreated()) {
                //描画レイヤー作成
                layer->create(Pos2D<Int32>(0, 0), this->windowSize_);
                this->layerNum_++;
            }

            //FBOに描画
            layer->beginDraw();
            glClearColor(red / 255.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            layer->endDraw();

            layer = &this->layerList_[1];
            if (!layer->isCreated()) {
                //描画レイヤー作成
                layer->create(Pos2D<Int32>(50, 50), Size<Int32>(500, 500));
                this->layerNum_++;
            }

            //FBOに描画
            layer->beginDraw();
            glClearColor(0.0F, 0.0F, blue / 255.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            layer->endDraw();

            //レイヤー表示更新
            this->updateLayers();

            red += 1.0F;
            if(red > 255.0F) {
                red = 0.0F;
            }
            blue += 1.0F;
            if(blue > 255.0F) {
                blue = 0.0F;
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
        GLuint unif_mvp = this->shader_.getUniformLocation("unif_mvp");
        GLuint unif_texture = this->shader_.getUniformLocation("unif_texture");

        //ビューポート設定
        glViewport(0, 0, this->windowSize_.getWidth(), this->windowSize_.getHeight());

        //使用するシェーダを指定
        glUseProgram(program);

        //MVP変換行列をシェーダへ転送
        glUniformMatrix4fv(unif_mvp, 1, GL_FALSE, this->projMat_[0]);

        //テクスチャマッピングを有効にする
        glEnable(GL_TEXTURE_2D);
        glEnableVertexAttribArray(attr_point);
        glEnableVertexAttribArray(attr_uv);

        for(Int32 iLayer = 0; iLayer < this->layerNum_; ++iLayer) {
            Layer& layer = this->layerList_[iLayer];

            glBindTexture(GL_TEXTURE_2D, layer.getTextureID());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //テクスチャユニット0を指定
            glUniform1i(unif_texture, 0);

            //頂点データ転送
            glBindBuffer(GL_ARRAY_BUFFER, layer.getCoordVetexID());
            glVertexAttribPointer(attr_point, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, layer.getTexCoordVetexID());
            glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

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