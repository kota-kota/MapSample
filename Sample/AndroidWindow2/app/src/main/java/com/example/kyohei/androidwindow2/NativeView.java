package com.example.kyohei.androidwindow2;

import android.content.Context;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class NativeView extends SurfaceView implements SurfaceHolder.Callback {

    private final static String LOG_TAG = "AndroidWindow";
    private SurfaceHolder mHolder;

    //コンストラクタ
    public NativeView(Context context) {
        super(context);

        //サーフェースフォルダーを取得
        mHolder = this.getHolder();
        //取得したサーフェースホルダーにコールバック機能を追加
        mHolder.addCallback(this);
    }

    //Surface作成時　SurfaceHolder.Callback
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(LOG_TAG, "NativeView::surfaceCreated");
    }

    //Surface解放時　SurfaceHolder.Callback
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(LOG_TAG, "NativeView::surfaceDestroyed");
    }

    //Surface変化時　SurfaceHolder.Callback
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
    }
}
