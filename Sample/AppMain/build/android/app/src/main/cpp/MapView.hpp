#ifndef INCLUDED_MAPVIEW_HPP
#define INCLUDED_MAPVIEW_HPP

#include "Type.hpp"
#include "LayerManager.hpp"
#include <GLES3/gl3.h>

namespace app {

    class MapView : public LayerView {

    public:
        //コンストラクタ
        MapView();
        //デストラクタ
        virtual ~MapView();
        //描画
        void draw();
    };

}

#endif //INCLUDED_MAPVIEW_HPP
