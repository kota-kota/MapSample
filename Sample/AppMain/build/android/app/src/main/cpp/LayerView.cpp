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
}