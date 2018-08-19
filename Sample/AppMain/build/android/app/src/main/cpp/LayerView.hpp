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
    };
}

#endif //INCLUDED_LAYERVIEW_HPP
