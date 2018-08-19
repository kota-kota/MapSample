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
            type_(NORMAL), isOnTouch_(false), lastTouchPos_()
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

    //タッチイベント処理
    bool MapView::onTouchEvent(const TouchEvent ev, const Pos2D<Float> pos)
    {
        LOGI("MapView::%s\n", __FUNCTION__);

        bool isProc = false;
        const Pos2D<Int32> touchPos(static_cast<Int32>(pos.getX()), static_cast<Int32>(pos.getY()));
        switch(ev) {
            case TOUCH_ON: {
                if(this->detectCollision(pos)) {
                    LOGI("MapView::%s TOUCH_ON (%f,%f)\n", __FUNCTION__, pos.getX(), pos.getY());
                    this->isOnTouch_ = true;
                    this->lastTouchPos_ = touchPos;
                    isProc = true;
                }
                break;
            }
            case TOUCH_OFF: {
                LOGI("MapView::%s TOUCH_OFF (%f,%f)\n", __FUNCTION__, pos.getX(), pos.getY());
                this->isOnTouch_ = false;
                this->lastTouchPos_ = Pos2D<Int32>();
                isProc = true;
                break;
            }
            case TOUCH_MOVE: {
                if(this->isOnTouch_) {
                    LOGI("MapView::%s TOUCH_MOVE (%f,%f)\n", __FUNCTION__, pos.getX(), pos.getY());
                    Pos2D<Int32> layerPos = this->getPos();
                    layerPos.moveX(touchPos.getX() - this->lastTouchPos_.getX());
                    layerPos.moveY(touchPos.getY() - this->lastTouchPos_.getY());
                    this->updatePos(layerPos);

                    this->lastTouchPos_ = touchPos;

                    isProc = true;
                }
                break;
            }
            default:
                break;
        }
        return isProc;
    }
}