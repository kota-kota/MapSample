#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include "Type.hpp"
#include "Shader.hpp"
#include "Math.hpp"

#include <thread>
#include <mutex>
#include <list>
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
        UInt32 id_;
        Pos2D<Int32> pos_;
        Size<Int32> size_;
        UInt32 fbo_[FBO_MAX];
        UInt32 vbo_[VBO_MAX];

    public:
        //コンストラクタ
        Layer(const UInt32 id);
        //デストラクタ
        ~Layer();
        //レイヤー作成
        ReturnCode create(const Size<Int32> size);
        //レイヤー破棄
        void destroy();
        //レイヤー描画
        void draw(const Shader& shader);
        //レイヤーに対する描画開始
        void beginDraw();
        //レイヤーに対する描画終了
        void endDraw();
        //当たり判定
        bool isCollision(const Float x, const Float y);
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


    class LayerView {
        //メンバ変数
        Layer* layer_;

    public:
        //コンストラクタ
        LayerView();
        //デストラクタ
        virtual ~LayerView();
        //ビュー作成
        ReturnCode create(Pos2D<Int32> pos, Size<Int32> size);
        //ビュー破棄
        void destroy();

    protected:
        //レイヤーに対する描画開始
        void beginDraw();
        //レイヤーに対する描画終了
        void endDraw();
    };


    class LayerManager {
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

        static const Int32 maxLayerNum_ = 3;
        Int32 layerNum_;
        Layer* layerList_[maxLayerNum_];
        Int32 touchLayer_;
        Pos2D<Int32> touchPos_;
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
        Layer* createLayer(const Size<Int32> size);
        //レイヤー破棄
        bool destroyLayer(Layer* const delLayer);
        //タッチイベント
        void procTouchEvent(TouchEvent ev, Float x, Float y);

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
