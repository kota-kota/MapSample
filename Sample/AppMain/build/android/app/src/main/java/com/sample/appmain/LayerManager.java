package com.sample.appmain;

import android.view.Surface;

public class LayerManager {

    //コンストラクタ
    public LayerManager() {
        LayerManager_create();
    }

    //破棄
    public void destroy() {
        LayerManager_destroy();
    }

    //表示更新開始
    public void start(Surface surface, int w, int h) {
        LayerManager_start(surface, w, h);
    }

    //表示更新停止
    public void stop() {
        LayerManager_stop();
    }

    //タッチイベント
    public void procTouchEvent_ON(float x, float y) {
        LayerManager_procTouchEvent_ON(x, y);
    }
    public void procTouchEvent_OFF(float x, float y) {
        LayerManager_procTouchEvent_OFF(x, y);
    }
    public void procTouchEvent_MOVE(float x, float y) {
        LayerManager_procTouchEvent_MOVE(x, y);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void LayerManager_create();
    public native void LayerManager_start(Surface surface, int w, int h);
    public native void LayerManager_stop();
    public native void LayerManager_destroy();
    public native void LayerManager_procTouchEvent_ON(float x, float y);
    public native void LayerManager_procTouchEvent_OFF(float x, float y);
    public native void LayerManager_procTouchEvent_MOVE(float x, float y);
}
