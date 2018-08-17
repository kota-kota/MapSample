#include "AppMain.hpp"
#include "LayerManager.hpp"
#include "Math.hpp"
#include "Logger.hpp"

namespace {
    //インスタンスハンドル
    static app::AppMain *g_instance_ = nullptr;
}

namespace app {

    //------------------------------------------------------------------------------------
    // AppMain

    //コンストラクタ
    AppMain::AppMain()
    {
        LOGI("AppMain::%s\n", __FUNCTION__);

        //Mathクラスのテスト関数
        app::test_Math();
    }

    //デストラクタ
    AppMain::~AppMain()
    {
        LOGI("AppMain::%s\n", __FUNCTION__);
    }

    //インスタンス作成
    void AppMain::create()
    {
        //自身のインスタンスを生成
        if(g_instance_ == nullptr) {
            g_instance_ = new AppMain();
        }
        LOGI("AppMain::%s g_instance:%p\n", __FUNCTION__, g_instance_);

        //レイヤー管理オブジェクトを生成
        LayerManager::create();
    }

    //インスタンス取得
    AppMain* AppMain::get()
    {
        return g_instance_;
    }

    //インスタンス破棄
    void AppMain::destroy()
    {
        LOGI("AppMain::%s g_instance:%p\n", __FUNCTION__, g_instance_);

        //レイヤー管理オブジェクトを破棄
        LayerManager::destroy();

        //自身のインスタンスを破棄
        if(g_instance_ != nullptr) {
            delete g_instance_;
            g_instance_ = nullptr;
        }
    }

    //アプリ開始
    void AppMain::start(void* native, Int32 w, Int32 h)
    {
        LOGI("AppMain::%s\n", __FUNCTION__);

        //表示更新の開始
        LayerManager* layerManager = LayerManager::get();
        LayerManager::get()->start(native, w, h);
    }

    //アプリ停
    void* AppMain::stop()
    {
        LOGI("AppMain::%s\n", __FUNCTION__);

        //表示更新の停止
        LayerManager* layerManager = LayerManager::get();
        return layerManager->stop();
    }

    //タッチイベント
    void AppMain::procTouchEvent(TouchEvent ev, Float x, Float y)
    {
        LayerManager* layerManager = LayerManager::get();
        layerManager->procTouchEvent(ev, x, y);
    }
}