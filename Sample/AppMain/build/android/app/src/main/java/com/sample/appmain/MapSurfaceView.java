package com.sample.appmain;

import android.content.Context;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MapSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private final static String LOG_TAG = "AppMain";
    //レイヤー管理オブジェクト
    LayerManager layerManager;

    //コンストラクタ
    public MapSurfaceView(Context context) {
        super(context);
        Log.i(LOG_TAG, "MapSurfaceView::MapSurfaceView");

        //レイヤー管理オブジェクトを生成
        layerManager = new LayerManager();

        //サーフェースフォルダーを取得
        SurfaceHolder holder = this.getHolder();
        //取得したサーフェースホルダーにコールバック機能を追加
        holder.addCallback(this);
    }

    //破棄
    public void destroy() {
        Log.i(LOG_TAG, "MapSurfaceView::destroy");
        layerManager.destroy();
    }

    //Surface作成時　SurfaceHolder.Callback
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(LOG_TAG, "MapSurfaceView::surfaceCreated");
    }

    //Surface変化時　SurfaceHolder.Callback
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.i(LOG_TAG, "MapSurfaceView::surfaceChanged format:" + format + " w:" + w + " h:" + h);
        //表示更新開始
        layerManager.start(holder.getSurface(), w, h);
    }

    //Surface解放時　SurfaceHolder.Callback
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(LOG_TAG, "MapSurfaceView::surfaceDestroyed");
        //表示更新停止
        layerManager.stop();
    }
}
