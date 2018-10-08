package com.sample.appmain;

import android.content.Context;
import android.graphics.PointF;
import android.text.method.Touch;
import android.util.Log;
import android.view.MotionEvent;

public class TouchGestureDetector {

    //リスナーインターフェース
    public interface TouchGestureListener {
        void onDown(TouchPoint touchPoints[], int iTouchPoint);
        void onUp(TouchPoint touchPoints[], int iTouchPoint);
        void onMove(TouchPoint touchPoints[], int iTouchPoint);
        void onDrag(Vector vec);
        void onFlick(TouchGestureDetector detector);
        void onPinch(TouchGestureDetector detector);
        void onTwist(TouchGestureDetector detector);
        void onTilt(TouchGestureDetector detector);
    }

    private final static String LOG_TAG = "AppMain";
    private final static int TOUCH_MAXNUM = 2;
    //リスナー
    private TouchGestureListener mListener = null;
    //タッチ座標管理
    private int mMaxTouchNum;   //最大タッチ数
    private int mTouchNum;      //タッチ数
    private TouchPoint mTouchPoints[]; //最新タッチ座標

    //コンストラクタ
    TouchGestureDetector(Context context, int maxTouchNum) {
        mMaxTouchNum = maxTouchNum;
        mTouchNum = 0;
        mTouchPoints = new TouchPoint[mMaxTouchNum];
        for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
            mTouchPoints[iPoint] = new TouchPoint();
        }
    }

    //リスナー登録
    public void setListener(TouchGestureListener listener) {
        this.mListener = listener;
    }

    //タッチイベント
    public boolean onTouchEvent(MotionEvent event) {
        if(this.mListener == null) {
            //リスナー未登録
            return false;
        }

        boolean ret = false;
        int eventAction = event.getActionMasked();
        switch (eventAction) {
            case MotionEvent.ACTION_DOWN:
                ret = actionDOWN(event);
                break;
            case MotionEvent.ACTION_UP:
                ret = actionUP(event);
                break;
            case MotionEvent.ACTION_POINTER_DOWN:
                ret = actionDOWN(event);
                break;
            case MotionEvent.ACTION_POINTER_UP:
                ret = actionUP(event);
                break;
            case MotionEvent.ACTION_MOVE:
                ret = actionMOVE(event);
                break;
            default:
                break;
        }

        return ret;
    }

    //ACTION_DOWNの処理
    private boolean actionDOWN(MotionEvent event) {
        boolean ret = false;
        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        float x = event.getX(pointerIndex);
        float y = event.getX(pointerIndex);
        for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
            ret = mTouchPoints[iPoint].down(pointerId, x, y);
            if(ret) {
                mListener.onDown(mTouchPoints, iPoint);
                mTouchNum++;
                break;
            }
        }
        return ret;
    }

    //ACTION_UPの処理
    private boolean actionUP(MotionEvent event) {
        boolean ret = false;
        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        float x = event.getX(pointerIndex);
        float y = event.getY(pointerIndex);
        for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
            ret = mTouchPoints[iPoint].up(pointerId, x, y);
            if(ret) {
                mListener.onUp(mTouchPoints, iPoint);
                mTouchNum--;
                break;
            }
        }
        return ret;
    }

    //ACTION_MOVEの処理
    private boolean actionMOVE(MotionEvent event) {
        boolean ret = false;

        //最新以外の未処理分を処理
        int histNum = event.getHistorySize();
        for(int h = 0; h < histNum; h++) {
            for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
                if(mTouchPoints[iPoint].enable()) {
                    int pointerIndex = event.findPointerIndex(mTouchPoints[iPoint].getId());
                    float x = event.getHistoricalX(pointerIndex, h);
                    float y = event.getHistoricalY(pointerIndex, h);
                    ret = mTouchPoints[iPoint].move(x, y);
                    if (ret) {
                        mListener.onMove(mTouchPoints, iPoint);
                    }
                }
            }

            ret = detectMoveAction();
        }

        //最新分を処理
        for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
            if(mTouchPoints[iPoint].enable()) {
                int pointerIndex = event.findPointerIndex(mTouchPoints[iPoint].getId());
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                ret = mTouchPoints[iPoint].move(x, y);
                if (ret) {
                    mListener.onMove(mTouchPoints, iPoint);
                }
            }
        }

        return ret;
    }

    private boolean detectMoveAction() {
        boolean ret = false;
        if(mTouchNum == 1) {
            //1点タッチ
            TouchPoint touchPoint = null;
            for(int iPoint = 0; iPoint < mMaxTouchNum; iPoint++) {
                if(mTouchPoints[iPoint].enable()) {
                    touchPoint = mTouchPoints[iPoint];
                    break;
                }
            }
            if(touchPoint != null) {
                Vector vec = new Vector(touchPoint.getPrePoint(), touchPoint.getLastPoint());
                mListener.onDrag(vec);
                ret = true;
            }
        }
        if(mTouchNum == 2) {
            //2点タッチ
            TouchPoint touchPoint1 = mTouchPoints[0];
            TouchPoint touchPoint2 = mTouchPoints[1];
            if((touchPoint1 != null) && (touchPoint2 != null)) {
                Vector vecP1P2 = new Vector(touchPoint1.getPrePoint(), touchPoint2.getPrePoint());
                Vector vecP1L1 = new Vector(touchPoint1.getPrePoint(), touchPoint1.getLastPoint());
                Vector vecP2L2 = new Vector(touchPoint2.getPrePoint(), touchPoint2.getLastPoint());
                if(vecP1L1.getLength() != 0.0f) {
                }
                if(vecP2L2.getLength() != 0.0f) {
                }
            }
        }
        return ret;
    }
}
