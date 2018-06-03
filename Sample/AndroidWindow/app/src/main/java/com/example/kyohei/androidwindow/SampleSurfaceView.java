package com.example.kyohei.androidwindow;

import android.content.Context;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class SampleSurfaceView
        extends SurfaceView
        implements SurfaceHolder.Callback {

    private final static String TAG = "AndroidWindow";

    private SurfaceHolder mHolder;

    //コンストラクタ
    public SampleSurfaceView(Context context, SurfaceView sv) {
        super(context);

        //サーフェースフォルダーを取得
        mHolder = sv.getHolder();
        //取得したサーフェースホルダーにコールバック機能を追加
        mHolder.addCallback(this);
    }

    //Surface作成時　SurfaceHolder.Callback
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "SampleSurfaceView::surfaceCreated");
    }

    //Surface解放時　SurfaceHolder.Callback
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "SampleSurfaceView::surfaceDestroyed");
        Jni.nativeDestroySurface();
    }

    //Surface変化時　SurfaceHolder.Callback
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        String log = "SampleSurfaceView::surfaceChanged format:" + format + " w:" + w + " h:" + h;
        Log.i(TAG, log);
        Jni.nativeCreateSurface(holder.getSurface(), w, h);
    }

    private void doDraw(SurfaceHolder holder) {
        Log.i(TAG, "SampleSurfaceView::doDraw");
    }

    private void onDraw(SurfaceHolder holder) {
        Log.i(TAG, "SampleSurfaceView::onDraw");
    }
}