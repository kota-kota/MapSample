#include "MapView.hpp"
#include "Logger.hpp"

namespace app {

    static Float green = 0.0F;

    //------------------------------------------------------------------------------------
    // MapView

    //コンストラクタ
    MapView::MapView()
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

        glClearColor(0.0F, green / 255.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();

        //描画終了
        this->endDraw();

        green += 1.0F;
        if(green > 255.0F) {
            green = 0.0F;
        }
    }
}