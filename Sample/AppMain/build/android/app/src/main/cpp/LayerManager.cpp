#include "LayerManager.hpp"
#include "MapView.hpp"
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
            "uniform highp mat4 unif_proj;\n"
            "uniform highp mat4 unif_mv;\n"
            "varying lowp vec2 vary_uv;\n"
            "void main() {\n"
            "    gl_Position = unif_proj * unif_mv * vec4( attr_point, 1.0 );\n"
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
            id_(0), pos_(), size_(), fbo_(), vbo_()
    {
        LOGI("Layer::%s\n", __FUNCTION__);
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
        LOGI("Layer::%s\n", __FUNCTION__);
    }

    //レイヤー作成済み判定
    bool Layer::isCreated()
    {
        bool ret = false;
        if(this->id_ != 0U) {
            ret = true;
        }
        return ret;
    }

    //レイヤー作成
    ReturnCode Layer::create(const UInt32 id, const Pos2D<Int32> pos, const Size<Int32> size)
    {
        LOGI("Layer::%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;

        //座標
        Float xmin = 0.0F;
        Float ymin = 0.0F;
        Float xmax = static_cast<Float>(size.getWidth());
        Float ymax = static_cast<Float>(size.getHeight());
        Float coords[] = {
                xmin, ymin,
                xmax, ymin,
                xmin, ymax,
                xmax, ymax,
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.getWidth(), size.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //デプスバッファ用のレンダーバッファを用意する
        glGenRenderbuffers(1, &this->fbo_[FboType::FBO_DEPTH]);
        glBindRenderbuffer(GL_RENDERBUFFER, this->fbo_[FboType::FBO_DEPTH]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.getWidth(), size.getHeight());
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

        LOGI("Layer::%s fbo:%d color:%d depth:%d status:0x%x\n", __FUNCTION__, this->fbo_[FboType::FBO], this->fbo_[FboType::FBO_COLOR], this->fbo_[FboType::FBO_DEPTH], status);
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
        LOGI("Layer::%s vbo_coord:%d vbo_texcoord:%d\n", __FUNCTION__, this->vbo_[VboType::VBO_COORD], this->vbo_[VboType::VBO_TEXCOORD]);

        //IDと位置とサイズを保持
        this->id_ = id;
        this->pos_ = pos;
        this->size_ = size;

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("Layer::%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //レイヤー破棄
    void Layer::destroy()
    {
        LOGI("Layer::%s fbo:%d color:%d depth:%d\n", __FUNCTION__, this->fbo_[FboType::FBO], this->fbo_[FboType::FBO_COLOR], this->fbo_[FboType::FBO_DEPTH]);
        glDeleteTextures(1, &this->fbo_[FboType::FBO_COLOR]);
        glDeleteRenderbuffers(1, &this->fbo_[FboType::FBO_DEPTH]);
        glDeleteFramebuffers(1, &this->fbo_[FboType::FBO]);
        LOGI("Layer::%s vbo_coord:%d vbo_texcoord:%d\n", __FUNCTION__, this->vbo_[VboType::VBO_COORD], this->vbo_[VboType::VBO_TEXCOORD]);
        glDeleteBuffers(VboType::VBO_MAX, &this->vbo_[0]);
    }

    //レイヤー描画
    void Layer::draw(const Shader& shader)
    {
        GLuint attr_point = shader.getAttrLocation("attr_point");
        GLuint attr_uv = shader.getAttrLocation("attr_uv");
        GLuint unif_mv = shader.getUniformLocation("unif_mv");
        GLuint unif_texture = shader.getUniformLocation("unif_texture");

        //テクスチャマッピングを有効にする
        glEnable(GL_TEXTURE_2D);
        glEnableVertexAttribArray(attr_point);
        glEnableVertexAttribArray(attr_uv);

        glBindTexture(GL_TEXTURE_2D, this->fbo_[FboType::FBO_COLOR]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //テクスチャユニット0を指定
        glUniform1i(unif_texture, 0);

        //モデルビュー変換行列をシェーダへ転送
        Matrix4F mv;
        mv.identify();
        mv.translate(static_cast<Float>(this->pos_.getX()), static_cast<Float>(this->pos_.getY()), 0.0F);
        mv.transpose();
        glUniformMatrix4fv(unif_mv, 1, GL_FALSE, mv[0]);

        //頂点データ転送
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_[VboType::VBO_COORD]);
        glVertexAttribPointer(attr_point, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_[VboType::VBO_TEXCOORD]);
        glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //描画
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);

        //テクスチャマッピングを無効にする
        glDisableVertexAttribArray(attr_point);
        glDisableVertexAttribArray(attr_uv);
        glDisable(GL_TEXTURE_2D);
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

    //当たり判定
    bool Layer::isCollision(const Float x, const Float y)
    {
        bool ret = false;

        Area<Float> area = this->calcDrawArea();
        if((area.getXMin() <= x) && (x <= area.getXMax()) &&
           (area.getYMin() <= y) && (y <= area.getYMax()))
        {
            //当たり
            ret = true;
        }

        return ret;
    }

    //レイヤーID取得
    UInt32 Layer::getId() const
    {
        return this->id_;
    }

    //レイヤー画面上位置取得
    Pos2D<Int32> Layer::getPos() const
    {
        return this->pos_;
    }

    //レイヤー画面上位置更新
    void Layer::updatePos(const Pos2D<Int32> pos)
    {
        this->pos_ = pos;
    }

    //レイヤーサイズ取得
    Size<Int32> Layer::getSize() const
    {
        return this->size_;
    }

    //描画エリアを計算
    Area<Float> Layer::calcDrawArea() const
    {
        Float xmin = static_cast<Float>(this->pos_.getX());
        Float ymin = static_cast<Float>(this->pos_.getY());
        Float xmax = xmin + static_cast<Float>(this->size_.getWidth());
        Float ymax = ymin + static_cast<Float>(this->size_.getHeight());
        return Area<Float>(xmin, ymin, xmax, ymax);
    }




    //------------------------------------------------------------------------------------
    // LayerManager

    //コンストラクタ
    LayerManager::LayerManager() :
            isTask_(false), isPause_(false), th_(), mtx_(), windowSize_(), native_(nullptr), projMat_(),
            eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglCtx_(EGL_NO_CONTEXT), eglWin_(EGL_NO_SURFACE),
            shader_(), layerNum_(0), layers_(), lastLayerId_(0), touchLayerId_(0), lastTouchPos_()
    {
        LOGI("LayerManager::%s\n", __FUNCTION__);

        //Mathクラスのテスト関数
        app::test_Math();

        //スレッド作成
        this->th_ = std::thread(&LayerManager::mainTask, this);
    }

    //デストラクタ
    LayerManager::~LayerManager()
    {
        LOGI("LayerManager::%s\n", __FUNCTION__);

        //タスク終了
        this->isTask_ = false;
        this->th_.join();
    }

    //インスタンス作成
    void LayerManager::create()
    {
        if(g_instance_ == nullptr) {
            g_instance_ = new LayerManager();
        }
        LOGI("LayerManager::%s g_instance:%p\n", __FUNCTION__, g_instance_);
    }

    //インスタンス取得
    LayerManager* LayerManager::get()
    {
        return g_instance_;
    }

    //インスタンス破棄
    void LayerManager::destroy()
    {
        LOGI("LayerManager::%s g_instance:%p\n", __FUNCTION__, g_instance_);
        if(g_instance_ != nullptr) {
            delete g_instance_;
            g_instance_ = nullptr;
        }
    }

    //表示更新開始
    void LayerManager::start(void* native, Int32 w, Int32 h)
    {
        LOGI("LayerManager::%s native:%p w:%d h:%d\n", __FUNCTION__, native, w, h);

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
        LOGI("LayerManager::%s native:%p\n", __FUNCTION__, this->native_);

        this->mtx_.lock();

        //表示資源のリセット
        void* ret = this->native_;
        this->native_ = nullptr;
        this->windowSize_.setWidth(0);
        this->windowSize_.setHeight(0);

        this->mtx_.unlock();
        return ret;
    }

    //レイヤー作成
    UInt32 LayerManager::createLayer(const Pos2D<Int32> pos, const Size<Int32> size)
    {
        UInt32 retLayerId;

        //空きがあるか
        if(this->layerNum_ < MAX_LAYER_NUM_) {
            //空きあり

            //レイヤーID発行
            UInt32 newId = this->lastLayerId_ + 1U;
            //レイヤー作成
            Layer layer;
            const ReturnCode rc = layer.create(newId, pos, size);
            if(rc == OK) {
                //作成成功

                //作成したレイヤーを保持
                this->layers_[this->layerNum_] = layer;
                this->layerNum_++;

                //最新レイヤーIDを更新
                this->lastLayerId_ = newId;

                //作成したレイヤーのレイヤーIDをリターン
                retLayerId = newId;
            }
            else {
                //作成失敗
                retLayerId = 0U;
            }
        }
        else {
            //空きなし
            retLayerId = 0U;
        }

        return retLayerId;
    }

    //レイヤー破棄
    void LayerManager::destroyLayer(const UInt32 layerid)
    {
        for(Int32 i = 0; i < this->layerNum_; i++) {
            if(this->layers_[i].getId() == layerid) {
                this->layers_[i].destroy();
                break;
            }
        }
    }

    //レイヤー取得
    Layer* LayerManager::getLayer(const UInt32 layerid)
    {
        Layer* layer = nullptr;
        for (Int32 i = 0; i < this->layerNum_; i++) {
            if (this->layers_[i].getId() == layerid) {
                layer = &this->layers_[i];
                break;
            }
        }
        return layer;
    }

    //タッチイベント
    void LayerManager::procTouchEvent(TouchEvent ev, Float x, Float y)
    {
        Pos2D<Int32> touchPos(static_cast<Int32>(x), static_cast<Int32>(y));
        switch(ev) {
            case TOUCH_ON: {
                LOGI("LayerManager::%s TOUCH_ON (%f,%f)\n", __FUNCTION__, x, y);
                for(Int32 i = this->layerNum_ - 1; i >= 0; i--) {
                    if(this->layers_[i].isCollision(x, y)) {
                        this->touchLayerId_ = this->layers_[i].getId();
                        break;
                    }
                }
                this->lastTouchPos_ = touchPos;
                break;
            }
            case TOUCH_OFF: {
                LOGI("LayerManager::%s TOUCH_OFF (%f,%f)\n", __FUNCTION__, x, y);
                this->touchLayerId_ = 0U;
                break;
            }
            case TOUCH_MOVE: {
                LOGI("LayerManager::%s TOUCH_MOVE (%f,%f)\n", __FUNCTION__, x, y);
                Layer* layer = this->getLayer(this->touchLayerId_);
                if(layer != nullptr) {
                    Pos2D<Int32> layerPos = layer->getPos();
                    layerPos.moveX(touchPos.getX() - this->lastTouchPos_.getX());
                    layerPos.moveY(touchPos.getY() - this->lastTouchPos_.getY());
                    layer->updatePos(layerPos);

                    this->lastTouchPos_ = touchPos;
                }
                break;
            }
            default:
                break;
        }
    }

    //メインタスク
    void LayerManager::mainTask()
    {
        LOGI("LayerManager::%s START\n", __FUNCTION__);
        ReturnCode retCode;

        MapView *mapView1 = new MapView();
        MapView *mapView2 = new MapView();

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
                    //LOGI("LayerManager::%s PAUSE\n", __FUNCTION__);
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
                LOGI("LayerManager::%s PAUSE -> RUN\n", __FUNCTION__);
                this->isPause_ = false;
            }
            else {
                //表示更新実施中
                if(native == nullptr) {
                    //ウィンドウがない→破棄された

                    //EGLウィンドウサーフェイスを破棄する
                    this->destroyWindowSurface();

                    //表示更新を停止する
                    LOGI("LayerManager::%s RUN -> PAUSE\n", __FUNCTION__);
                    this->isPause_ = true;
                    continue;
                }
                //ウィンドウがある→実施中を継続
                //LOGI("LayerManager::%s RUN\n", __FUNCTION__);
            }

            if(!this->shader_.isCreated()) {
                //シェーダ作成
                this->shader_.create(vertex_fbo, fragment_fbo);
            }

            //レイヤー登録
            if(this->layerNum_ == 0) {
                mapView1->create(Pos2D<Int32>(0, 0), this->windowSize_);
                mapView2->create(Pos2D<Int32>(50, 50), Size<Int32>(500, 500));
                mapView2->changeMapType(MapView::MapType::REAL);
            }

            //描画
            mapView1->draw();
            mapView2->draw();

            //レイヤー表示更新
            this->updateLayers();

            //待ち
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("LayerManager::%s !!!ERROR!!!\n", __FUNCTION__);
        }
        //EGL資源の解放
        this->destroyWindowSurface();
        this->destroyEGL();

        //タスク終了
        LOGI("LayerManager::%s END (retCode:%d)\n", __FUNCTION__, retCode);
    }

    //EGL資源作成
    ReturnCode LayerManager::createEGL()
    {
        LOGI("LayerManager::%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;
        EGLint major = 0, minor = 0;
        EGLint numConfigs = 0;

        //EGLディスプレイを取得
        this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        LOGI("LayerManager::%s eglGetDisplay dpy:%p (err:0x%x)\n", __FUNCTION__, this->eglDpy_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGL初期化
        (void)eglInitialize(this->eglDpy_, &major, &minor);
        LOGI("LayerManager::%s eglInitialize egl:%d.%d (err:0x%x)\n", __FUNCTION__, major, minor, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGLコンフィグを取得
        (void)eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numConfigs);
        LOGI("LayerManager::%s eglChooseConfig cfg:%p num:%d (err:0x%x)\n", __FUNCTION__, this->eglCfg_, numConfigs, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //EGLコンテキストを作成
        this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
        LOGI("LayerManager::%s eglCreateContext ctx:%p (err:0x%x)\n", __FUNCTION__, this->eglCtx_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("LayerManager::%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //EGL資源解放
    void LayerManager::destroyEGL()
    {
        if (this->eglCtx_ != EGL_NO_CONTEXT) {
            eglDestroyContext(this->eglDpy_, this->eglCtx_);
            LOGI("LayerManager::%s eglDestroyContext ctx:%p (err:0x%x)\n", __FUNCTION__, this->eglCtx_, eglGetError());
            this->eglCtx_ = EGL_NO_CONTEXT;
        }
        if (this->eglDpy_ != EGL_NO_DISPLAY) {
            //TODO: eglTerminate(this->eglDpy_);
            LOGI("LayerManager::%s eglTerminate dpy:%p (err:0x%x)\n", __FUNCTION__, this->eglDpy_, eglGetError());
            this->eglDpy_ = EGL_NO_DISPLAY;
        }
    }

    //EGLウィンドウサーフェイス作成
    ReturnCode LayerManager::createWindowSurface(void* native)
    {
        ReturnCode retCode = NG_ERROR;

        this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)native, nullptr);
        LOGI("LayerManager::%s eglCreateWindowSurface win:%p (err:0x%x)\n", __FUNCTION__, this->eglWin_, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        //カレント
        (void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
        LOGI("LayerManager::%s eglMakeCurrent bind (err:0x%x)\n", __FUNCTION__, eglGetError());
        if(eglGetError() != EGL_SUCCESS) { goto END; }

        retCode = OK;

        END:
        if(retCode != OK) {
            LOGE("LayerManager::%s !!!ERROR!!!\n", __FUNCTION__);
        }
        return retCode;
    }

    //EGLウィンドウサーフェイス解放
    void LayerManager::destroyWindowSurface()
    {
        (void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        LOGI("LayerManager::%s eglMakeCurrent unbind (err:0x%x)\n", __FUNCTION__, eglGetError());

        if (this->eglWin_ != EGL_NO_SURFACE) {
            eglDestroySurface(this->eglDpy_, this->eglWin_);
            LOGI("LayerManager::%s eglDestroySurface win:%p (err:0x%x)\n", __FUNCTION__, this->eglWin_, eglGetError());
            this->eglWin_ = EGL_NO_SURFACE;
        }
    }

    //レイヤー表示更新
    void LayerManager::updateLayers()
    {
        //ビューポート設定
        glViewport(0, 0, this->windowSize_.getWidth(), this->windowSize_.getHeight());

        //画面クリア
        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        //使用するシェーダを指定
        GLuint program = this->shader_.getProgramId();
        glUseProgram(program);

        //プロジェクション変換行列をシェーダへ転送
        GLuint unif_proj = this->shader_.getUniformLocation("unif_proj");
        glUniformMatrix4fv(unif_proj, 1, GL_FALSE, this->projMat_[0]);

        //各レイヤーの描画
        for(Int32 i = 0; i < this->layerNum_; i++) {
            this->layers_[i].draw(this->shader_);
        }

        //表示更新
        (void)eglSwapBuffers(this->eglDpy_, this->eglWin_);
    }
}