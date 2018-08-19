#include "LayerView.hpp"
#include "LayerManager.hpp"
#include "Logger.hpp"

namespace app {

    //------------------------------------------------------------------------------------
    // LayerView

    //コンストラクタ
    LayerView::LayerView() :
            layerId_(0)
    {
        LOGI("Layer::%s\n", __FUNCTION__);
    }

    //デストラクタ
    LayerView::~LayerView()
    {
        LOGI("Layer::%s\n", __FUNCTION__);
        this->destroy();
    }

    //ビュー作成
    ReturnCode LayerView::create(const Pos2D<Int32> pos, const Size<Int32> size)
    {
        ReturnCode rc = NG_ERROR;

        //レイヤー作成
        LayerManager* layerManager = LayerManager::get();
        this->layerId_ = layerManager->createLayer(pos, size);
        if(this->layerId_ != 0U) {
            rc = OK;
        }

        Layer* layer = layerManager->getLayer(this->layerId_);
        layer->setListenerOnTouchEvent(this, this->callbackTouchEvent);

        return rc;
    }

    //ビュー破棄
    void LayerView::destroy()
    {
        //レイヤー破棄
        LayerManager* layerManager = LayerManager::get();
        layerManager->destroyLayer(this->layerId_);
        this->layerId_ = 0U;
    }

    //レイヤーに対する描画開始
    void LayerView::beginDraw()
    {
        //レイヤー取得
        LayerManager* layerManager = LayerManager::get();
        Layer* layer = layerManager->getLayer(this->layerId_);
        if(layer != nullptr) {
            //描画開始
            layer->beginDraw();
        }
    }

    //レイヤーに対する描画終了
    void LayerView::endDraw()
    {
        //レイヤー取得
        LayerManager* layerManager = LayerManager::get();
        Layer* layer = layerManager->getLayer(this->layerId_);
        if(layer != nullptr) {
            //描画終了
            layer->endDraw();
        }
    }

    //指定座標がビュー範囲内か検出
    bool LayerView::detectCollision(const Pos2D<Float> pos)
    {
        bool detect = false;
        //レイヤー取得
        LayerManager* layerManager = LayerManager::get();
        Layer* layer = layerManager->getLayer(this->layerId_);
        if(layer != nullptr) {
            detect = layer->detectCollision(pos);
        }
        return detect;
    }

    //ビューの画面上位置取得
    Pos2D<Int32> LayerView::getPos() const
    {
        Pos2D<Int32> pos;
        //レイヤー取得
        LayerManager* layerManager = LayerManager::get();
        Layer* layer = layerManager->getLayer(this->layerId_);
        if(layer != nullptr) {
            pos = layer->getPos();
        }
        return pos;
    }

    //ビューの画面上位置更新
    void LayerView::updatePos(const Pos2D<Int32> pos)
    {
        //レイヤー取得
        LayerManager* layerManager = LayerManager::get();
        Layer* layer = layerManager->getLayer(this->layerId_);
        if(layer != nullptr) {
            layer->updatePos(pos);
        }
    }

    //タッチイベント処理
    bool LayerView::onTouchEvent(const TouchEvent ev, const Pos2D<Float> pos)
    {
        LOGI("LayerView::%s\n", __FUNCTION__);
        return false;
    }

    //タッチイベントコールバック関数
    bool LayerView::callbackTouchEvent(void* obj, const TouchEvent ev, const Pos2D<Float> pos)
    {
        return static_cast<LayerView*>(obj)->onTouchEvent(ev, pos);
    }
}