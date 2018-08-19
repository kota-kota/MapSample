#include "MapView.hpp"
#include "Logger.hpp"
#include <GLES3/gl3.h>

namespace app {

    static Float red = 0.0F;
    static Float green = 0.0F;

    //------------------------------------------------------------------------------------
    // MapView

    //コンストラクタ
    MapView::MapView() :
            type_(NORMAL)
    {
        LOGI("MapView::%s\n", __FUNCTION__);
    }

    //デストラクタ
    MapView::~MapView()
    {
        LOGI("MapView::%s\n", __FUNCTION__);
    }

    //描画
    void MapView::draw()
    {
        //描画開始
        this->beginDraw();

        if(this->type_ == NORMAL) {
            glClearColor(0.0F, green / 255.0F, 0.0F, 1.0F);
        }
        else if(this->type_ == REAL) {
            glClearColor(red / 255.0F, 0.0F, 0.0F, 1.0F);
        }
        else {
            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();

        //描画終了
        this->endDraw();

        green += 1.0F;
        if(green > 255.0F) {
            green = 0.0F;
        }
        red += 1.0F;
        if(red > 255.0F) {
            red = 0.0F;
        }
    }

    //地図種別を変更
    void MapView::changeMapType(const MapType type)
    {
        this->type_ = type;
    }
}