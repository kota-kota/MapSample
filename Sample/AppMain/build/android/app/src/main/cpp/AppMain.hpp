#ifndef INCLUDED_APPMAIN_HPP
#define INCLUDED_APPMAIN_HPP

#include "Type.hpp"

namespace app {

    class AppMain {
    private:
        //コンストラクタ
        AppMain();
        //デストラクタ
        ~AppMain();

    public:
        //インスタンス作成
        static void create();
        //インスタンス取得
        static AppMain* get();
        //インスタンス破棄
        static void destroy();
        //アプリ開始
        void start(void* native, Int32 w, Int32 h);
        //アプリ停止
        void* stop();
        //タッチイベント
        void procTouchEvent(TouchEvent ev, Float x, Float y);
    };
}

#endif //INCLUDED_APPMAIN_HPP
