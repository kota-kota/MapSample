package com.sample.appmain;

import android.view.Surface;

public class MapView {

    //コンストラクタ
    public MapView() {
        createMapView();
    }

    //開始
    public void start(Surface surface, int w, int h) {
        startMapView(surface, w, h);
    }

    //停止
    public void stop() {
        stopMapView();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void createMapView();
    public native void startMapView(Surface surface, int w, int h);
    public native void stopMapView();
}
