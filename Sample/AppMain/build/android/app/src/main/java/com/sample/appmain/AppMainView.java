package com.sample.appmain;

import android.content.Context;
import android.graphics.PointF;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import java.util.Locale;

public class AppMainView extends SurfaceView implements SurfaceHolder.Callback, TouchGestureDetector.TouchGestureListener {

    private final static String LOG_TAG = "AppMain";

    //タッチジェスチャ検出用
    TouchGestureDetector mTouchGestureDetector;
    //デバッグ表示用
    private int mTextViewNum;
    private TextView mTextViews[];

    //コンストラクタ
    public AppMainView(Context context) {
        super(context);
        Log.i(LOG_TAG, "AppMainView::AppMainView");

        //サーフェースフォルダーを取得
        SurfaceHolder holder = this.getHolder();
        //取得したサーフェースホルダーにコールバック機能を追加
        holder.addCallback(this);

        //タッチジェスチャ検出オブジェクト生成
        mTouchGestureDetector = new TouchGestureDetector(context, 2);
        //リスナー登録
        mTouchGestureDetector.setListener(this);
    }

    //デバッグ表示用テキストビュー設定
    public void setDebugTextView(TextView textViews[], int textViewNum) {
        mTextViewNum = textViewNum;
        mTextViews = textViews;
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
        return mTouchGestureDetector.onTouchEvent(event);
    }

    public void onDown(TouchPoint touchPoints[], int iTouchPoint) {
        //Log.i(LOG_TAG, "onDown");
        if(touchPoints[iTouchPoint] != null) {
            if((iTouchPoint < mTextViewNum) && (mTextViews[iTouchPoint] != null)) {
                PointF point = touchPoints[iTouchPoint].getLastPoint();
                String text = String.format(Locale.US, "onDown\t\tx:%4.1f y:%4.1f", point.x, point.y);
                mTextViews[iTouchPoint].setText(text);
            }
        }
    }
    public void onUp(TouchPoint touchPoints[], int iTouchPoint) {
        //Log.i(LOG_TAG, "onUp");
        if(touchPoints[iTouchPoint] != null) {
            if((iTouchPoint < mTextViewNum) && (mTextViews[iTouchPoint] != null)) {
                PointF point = touchPoints[iTouchPoint].getLastPoint();
                String text = String.format(Locale.US, "onUp\t\t\t\tx:%4.1f y:%4.1f", point.x, point.y);
                mTextViews[iTouchPoint].setText(text);
            }
        }
    }
    public void onMove(TouchPoint touchPoints[], int iTouchPoint) {
        //Log.i(LOG_TAG, "onMove");
        if(touchPoints[iTouchPoint] != null) {
            if((iTouchPoint < mTextViewNum) && (mTextViews[iTouchPoint] != null)) {
                PointF point = touchPoints[iTouchPoint].getLastPoint();
                String text = String.format(Locale.US, "onMove\t\tx:%4.1f y:%4.1f", point.x, point.y);
                mTextViews[iTouchPoint].setText(text);
            }
        }
    }
    public void onDrag(Vector vec) {
        Log.i(LOG_TAG, "onDrag");
        String text = String.format(Locale.US, "onDrag\t\tx:%4.1f y:%4.1f", vec.getX(), vec.getY());
        mTextViews[2].setText(text);
    }
    public void onFlick(TouchGestureDetector detector) {
        Log.i(LOG_TAG, "onFlick");
    }
    public void onPinch(TouchGestureDetector detector) {
        Log.i(LOG_TAG, "onPinch");
    }
    public void onTwist(TouchGestureDetector detector) {
        Log.i(LOG_TAG, "onTwist");
    }
    public void onTilt(TouchGestureDetector detector) {
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
