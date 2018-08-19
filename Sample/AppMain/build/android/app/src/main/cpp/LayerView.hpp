#ifndef INCLUDED_LAYERVIEW_HPP
#define INCLUDED_LAYERVIEW_HPP

#include "Type.hpp"

namespace app {

    class LayerView {
        //メンバ変数
        UInt32 layerId_;

    public:
        //コンストラクタ
        LayerView();
        //デストラクタ
        virtual ~LayerView();
        //ビュー作成
        ReturnCode create(const Pos2D<Int32> pos, const Size<Int32> size);
        //ビュー破棄
        void destroy();

    protected:
        //レイヤーに対する描画開始
        void beginDraw();
        //レイヤーに対する描画終了
        void endDraw();
        //指定座標がビュー範囲内か検出
        bool detectCollision(const Pos2D<Float> pos);
        //ビューの画面上位置取得
        Pos2D<Int32> getPos() const;
        //ビューの画面上位置更新
        void updatePos(const Pos2D<Int32> pos);
        //タッチイベント処理
        virtual bool onTouchEvent(const TouchEvent ev, const Pos2D<Float> pos);

    public:
        //タッチイベントコールバック関数
        static bool callbackTouchEvent(void* obj, const TouchEvent ev, const Pos2D<Float> pos);
    };
}

#endif //INCLUDED_LAYERVIEW_HPP
