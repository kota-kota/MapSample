#ifndef INCLUDED_MAPVIEW_HPP
#define INCLUDED_MAPVIEW_HPP

#include "Type.hpp"
#include "LayerView.hpp"

namespace app {

    class MapView : public LayerView {
    public:
        //地図種別
        enum MapType {
            NORMAL,
            REAL,
        };

    private:
        MapType type_;

    public:
        //コンストラクタ
        MapView();
        //デストラクタ
        virtual ~MapView();
        //描画
        void draw();
        //地図種別を変更
        void changeMapType(const MapType type);
    };

}

#endif //INCLUDED_MAPVIEW_HPP
