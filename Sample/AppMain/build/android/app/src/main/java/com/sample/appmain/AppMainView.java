package com.sample.appmain;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class AppMainView extends SurfaceView implements SurfaceHolder.Callback, TouchManager.TouchManagerListener {

    private final static String LOG_TAG = "AppMain";
    //タッチポインタID記憶用
    private int pointerId1 = -1;
    private int pointerId2 = -1;

    TouchManager touchManager;

    //コンストラクタ
    public AppMainView(Context context) {
        super(context);
        Log.i(LOG_TAG, "AppMainView::AppMainView");

        //サーフェースフォルダーを取得
        SurfaceHolder holder = this.getHolder();
        //取得したサーフェースホルダーにコールバック機能を追加
        holder.addCallback(this);

        //タッチマネージャー
        touchManager = new TouchManager(context);
        touchManager.setListener(this);
    }

    //生成
    public void create() {
        Log.i(LOG_TAG, "AppMainView::create");
        //アプリ作成
        AppMain_create();
    }

    //破棄
    public void destroy() {
        Log.i(LOG_TAG, "AppMainView::destroy");
        //アプリ破棄
        AppMain_destroy();
    }

    //Surface作成時　SurfaceHolder.Callback
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(LOG_TAG, "AppMainView::surfaceCreated");
    }

    //Surface変化時　SurfaceHolder.Callback
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.i(LOG_TAG, "AppMainView::surfaceChanged format:" + format + " w:" + w + " h:" + h);
        //アプリ開始
        AppMain_start(holder.getSurface(), w, h);
    }

    //Surface解放時　SurfaceHolder.Callback
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(LOG_TAG, "AppMainView::surfaceDestroyed");
        //アプリ停止
        AppMain_stop();
    }

    @Override
    public boolean performClick() {
        super.performClick();
        return true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        performClick();
        return touchManager.onTouchEvent(event);
    }

    public void onTouchDown(float x, float y) {
        Log.i(LOG_TAG, "onTouchDown (" + x + "," + y + ")");
    }
    public void onTouchUp(float x, float y) {
        Log.i(LOG_TAG, "onTouchUp (" + x + "," + y + ")");
    }
    public void onDrag(float x, float y) {
        Log.i(LOG_TAG, "onDrag (" + x + "," + y + ")");
    }
    public void onFlick() {
        Log.i(LOG_TAG, "onFlick");
    }
    public void onPinch() {
        Log.i(LOG_TAG, "onPinch");
    }
    public void onTwist() {
        Log.i(LOG_TAG, "onTwist");
    }
    public void onTilt() {
        Log.i(LOG_TAG, "onTilt");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void AppMain_create();
    public native void AppMain_start(Surface surface, int w, int h);
    public native void AppMain_stop();
    public native void AppMain_destroy();
    public native void AppMain_procTouchEvent_ON(float x, float y);
    public native void AppMain_procTouchEvent_OFF(float x, float y);
    public native void AppMain_procTouchEvent_MOVE(float x, float y);
}
