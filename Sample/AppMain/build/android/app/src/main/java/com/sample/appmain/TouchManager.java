package com.sample.appmain;

import android.content.Context;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import java.util.EventListener;

public class TouchManager implements GestureDetector.OnGestureListener, ScaleGestureDetector.OnScaleGestureListener {

    //リスナーインターフェース
    public interface TouchManagerListener extends EventListener {
        void onTouchDown(float x, float y);
        void onTouchUp(float x, float y);
        void onDrag(float x, float y);
        void onFlick();
        void onPinch();
        void onTwist();
        void onTilt();
    }

    private final static String LOG_TAG = "AppMain";
    private TouchManagerListener listener = null;
    private GestureDetector gesture;
    private ScaleGestureDetector scaleGesture;
    private float beginFocusX;
    private float beginFocusY;
    private float beginAngle;
    private float prevAngle;
    private boolean isTwist;
    private boolean isTilt;

    //コンストラクタ
    public TouchManager(Context context) {
        gesture = new GestureDetector(context, this);
        scaleGesture = new ScaleGestureDetector(context, this);
    }

    //リスナー登録
    public void setListener(TouchManagerListener listener) {
        this.listener = listener;
    }

    //タッチイベント
    public boolean onTouchEvent(MotionEvent event) {
        if(this.listener == null) {
            //リスナー未登録
            return false;
        }
        switch(event.getPointerCount()) {
            case 1:
                //1点タッチ
                doSingleTouch(event);
                break;
            case 2:
                //2点タッチ
                doDoubleTouch(event);
                break;
            default:
                //フェールで2点タッチとする
                doDoubleTouch(event);
                break;
        }
        gesture.onTouchEvent(event);
        if(!isTilt) {
            //チルト中はピンチ判定しない
            scaleGesture.onTouchEvent(event);
        }
        return true;
    }

    //ScaleGestureDetector: ピンチ開始
    public boolean onScaleBegin(ScaleGestureDetector detector) {
        Log.i(LOG_TAG, "onScaleBegin");
        return true;
    }

    //ScaleGestureDetector: ピンチ終了
    public void onScaleEnd(ScaleGestureDetector detector) {
        Log.i(LOG_TAG, "onScaleEnd");
    }

    //ScaleGestureDetector: ピンチ中
    public boolean onScale(ScaleGestureDetector detector) {
        Log.i(LOG_TAG, "onScale");
        listener.onPinch();
        return true;
    }

    //GestureDetector: タッチON
    public boolean onDown(MotionEvent event) {
        //ACTION_DOWNで拾うため何もしない
        return true;
    }

    //GestureDetector: タッチOFF
    public boolean onSingleTapUp(MotionEvent event) {
        //ACTION_UPで拾うため何もしない
        return true;
    }

    //GestureDetector: タッチON後に少し留まった
    public void onShowPress(MotionEvent event) {
        //使い道がわからないので何もしない
    }

    //GestureDetector: 長押し検知
    public void onLongPress(MotionEvent event) {
        //何もしない
    }

    //GestureDetector: スクロール
    public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
        //ACTION_MOVEで拾うため何もしない
        return true;
    }

    //GestureDetector: フリック
    public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
        Log.i(LOG_TAG, "onFling");
        listener.onFlick();
        return true;
    }

    //1点タッチ
    private void doSingleTouch(MotionEvent event) {
        float x = event.getX(0);
        float y = event.getY(0);
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                //Log.i(LOG_TAG, "ACTION_DOWN (" + x + "," + y + ")");
                listener.onTouchDown(x, y);
                break;
            case MotionEvent.ACTION_UP:
                //Log.i(LOG_TAG, "ACTION_UP (" + x + "," + y + ")");
                listener.onTouchUp(x, y);
                break;
            case MotionEvent.ACTION_MOVE:
                //Log.i(LOG_TAG, "ACTION_MOVE (" + x + "," + y + ")");
                listener.onDrag(x, y);
                break;
            default:
                break;
        }
    }

    //2点タッチ
    private void doDoubleTouch(MotionEvent event) {
        float x1 = event.getX(0);
        float y1 = event.getY(0);
        float x2 = event.getX(1);
        float y2 = event.getY(1);
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_POINTER_DOWN:
                //Log.i(LOG_TAG, "ACTION_POINTER_DOWN (" + x1 + "," + y1 + ") (" + x2 + "," + y2 + ")");
                beginFocusX = getFocusX(x1, x2);
                beginFocusY = getFocusY(y1, y2);
                beginAngle = getAngle(x1, y1, x2, y2);
                prevAngle = beginAngle;
                isTwist = false;
                isTilt = false;
                break;
            case MotionEvent.ACTION_POINTER_UP:
                //Log.i(LOG_TAG, "ACTION_POINTER_UP (" + x1 + "," + y1 + ") (" + x2 + "," + y2 + ")");
                beginFocusX = 0.0f;
                beginFocusY = 0.0f;
                beginAngle = 0.0f;
                prevAngle = 0.0f;
                isTwist = false;
                isTilt = false;
                break;
            case MotionEvent.ACTION_MOVE:
                //Log.i(LOG_TAG, "ACTION_MOVE (" + x1 + "," + y1 + ") (" + x2 + "," + y2 + ")");
                float focusX = getFocusX(x1, x2);
                float focusY = getFocusY(y1, y2);
                float angle = getAngle(x1, y1, x2, y2);
                if(!isTilt) {
                    //チルトじゃなければツイスト判定
                    if (isTwist) {
                        //ツイスト
                        Log.i(LOG_TAG, "TWIST prevAngle:" + prevAngle + ",angle:" + angle);
                        listener.onTwist();
                    }
                    else {
                        if (Math.abs(angle - beginAngle) > 5) {
                            //5度以上回転させたらツイスト判定
                            isTwist = true;
                        }
                    }
                }

                if(!isTwist) {
                    //ツイストじゃなければチルト判定
                    float dist = getDistance(focusX, focusY, beginFocusX, beginFocusY);
                    if(isTilt) {
                        //チルト
                        Log.i(LOG_TAG, "TILT dist:" + dist);
                        listener.onTilt();
                    }
                    else {
                        if(dist > 50.0f) {
                            //50dot以上動かしたらチルト
                            isTilt = true;
                        }
                    }
                }
                prevAngle = angle;
                break;
            default:
                break;
        }
    }

    //2点間の中心座標
    private float getFocusX(float x1, float x2) {
        return (x1 + x2) / 2.0f;
    }
    private float getFocusY(float y1, float y2) {
        return (y1 + y2) / 2.0f;
    }

    //2点間の距離
    private float getDistance(float x1, float y1, float x2, float y2) {
        double dist = Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        return (float)dist;
    }

    //2点間の角度
    private float getAngle(float x1, float y1, float x2, float y2) {
        double angle = Math.atan2(y2 - y1, x2 - x1) * 180d / Math.PI;
        return (float)angle;
    }
}
