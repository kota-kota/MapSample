#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include "Type.hpp"
#include "Shader.hpp"
#include "Math.hpp"

#include <thread>
#include <mutex>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

namespace app {

    class Layer {
        //メンバ変数
        enum FboType {
            FBO,
            FBO_COLOR,
            FBO_DEPTH,
            FBO_MAX,
        };
        enum VboType {
            VBO_COORD,
            VBO_TEXCOORD,
            VBO_MAX,
        };
        UInt32 id_;
        Pos2D<Int32> pos_;
        Size<Int32> size_;
        UInt32 fbo_[FBO_MAX];
        UInt32 vbo_[VBO_MAX];

        void* obj_;
        bool (*func_)(void* obj, const TouchEvent, const Pos2D<Float>);

    public:
        //コンストラクタ
        Layer();
        //デストラクタ
        ~Layer();
        //レイヤー作成済み判定
        bool isCreated();
        //レイヤー作成
        ReturnCode create(const UInt32 id, const Pos2D<Int32> pos, const Size<Int32> size);
        //レイヤー破棄
        void destroy();
        //レイヤー描画
        void draw(const Shader& shader);
        //レイヤーに対する描画開始
        void beginDraw();
        //レイヤーに対する描画終了
        void endDraw();

        //タッチイベントリスナー設定
        void setListenerOnTouchEvent(void* obj, bool (*func)(void* obj, const TouchEvent, const Pos2D<Float>));
        //タッチイベント処理
        bool onTouchEvent(const TouchEvent ev, const Pos2D<Float> pos);

        //指定座標がレイヤー範囲内か検出
        bool detectCollision(const Pos2D<Float> pos);

        //レイヤーID取得
        UInt32 getId() const;
        //レイヤー画面上位置取得
        Pos2D<Int32> getPos() const;
        //レイヤー画面上位置更新
        void updatePos(const Pos2D<Int32> pos);
        //レイヤーサイズ取得
        Size<Int32> getSize() const;

    private:
        //描画エリアを計算
        Area<Float> calcDrawArea() const;
    };

    class LayerManager {
        //定数
        static const Int32 MAX_LAYER_NUM_ = 8;

        //メンバ変数
        bool isTask_;
        bool isPause_;
        std::thread th_;
        std::mutex mtx_;

        Size<Int32> windowSize_;
        void* native_;
        Matrix4F projMat_;
        EGLDisplay eglDpy_;
        EGLConfig  eglCfg_;
        EGLContext eglCtx_;
        EGLSurface eglWin_;

        Shader shader_;

        Int32 layerNum_;
        Layer layers_[MAX_LAYER_NUM_];
        UInt32 lastLayerId_;

    private:
        //コンストラクタ
        LayerManager();
        //デストラクタ
        ~LayerManager();

    public:
        //インスタンス生成
        static void create();
        //インスタンス取得
        static LayerManager* get();
        //インスタンス破棄
        static void destroy();
        //表示更新開始
        void start(void* native, Int32 w, Int32 h);
        //表示更新停止
        void* stop();
        //レイヤー作成
        UInt32 createLayer(const Pos2D<Int32> pos, const Size<Int32> size);
        //レイヤー破棄
        void destroyLayer(const UInt32 layerid);
        //レイヤー取得
        Layer* getLayer(const UInt32 layerid);
        //タッチイベント処理
        void onTouchEvent(const TouchEvent ev, const Pos2D<Float> pos);

    private:
        //メインタスク
        void mainTask();
        //EGL資源作成
        ReturnCode createEGL();
        //EGL資源解放
        void destroyEGL();
        //EGLウィンドウサーフェイス作成
        ReturnCode createWindowSurface(void* native);
        //EGLウィンドウサーフェイス解放
        void destroyWindowSurface();
        //レイヤー表示更新
        void updateLayers();
    };
}

#endif //INCLUDED_LAYERMANAGER_HPP
