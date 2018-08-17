#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include "Type.hpp"
#include "Shader.hpp"
#include "Math.hpp"

#include <list>
#include <thread>
#include <mutex>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

namespace app {

    class Layer {
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

        //メンバ変数
        Pos2D<Int32> pos_;
        Size<Int32> size_;
        UInt32 fbo_[FBO_MAX];
        UInt32 vbo_[VBO_MAX];

    public:
        //コンストラクタ
        Layer();
        //デストラクタ
        ~Layer();
        //テクスチャID取得
        UInt32 getTextureID();
        //座標頂点データID取得
        UInt32 getCoordVetexID();
        //テクスチャ座標頂点データID取得
        UInt32 getTexCoordVetexID();
        //モデルビュー変換行列を取得
        Matrix4F getModelViewMatrix();
        //レイヤー作成チェック
        bool isCreated();
        //レイヤー作成
        ReturnCode create(Pos2D<Int32> pos, Size<Int32> size);
        //レイヤー破棄
        void destroy();
        //レイヤーに対する描画開始
        void beginDraw();
        //レイヤーに対する描画終了
        void endDraw();
        //画面上位置取得
        Pos2D<Int32> getPos() const;
        //画面上位置更新
        void updatePos(const Pos2D<Int32> pos);
        //当たり判定
        bool isCollision(const Float x, const Float y);

    private:
        //描画エリアを計算
        Area<Float> calcDrawArea() const;
    };

    class LayerManager {
    public:
        enum TouchEvent {
            TOUCH_ON,
            TOUCH_OFF,
            TOUCH_MOVE,
        };

    private:
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

        static const Int32 maxLayerNum_ = 2;
        Int32 layerNum_;
        Layer layerList_[maxLayerNum_];
        Int32 touchLayer_;
        Pos2D<Int32> touchPos_;

    public:
        //インスタンス取得
        static LayerManager* getInstance();
        //インスタンス削除
        static void delInstance();
        //表示更新開始
        void start(void* native, Int32 w, Int32 h);
        //表示更新停止
        void* stop();
        //タッチイベント
        void procTouchEvent(TouchEvent ev, Float x, Float y);

    private:
        //コンストラクタ
        LayerManager();
        //デストラクタ
        ~LayerManager();
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
