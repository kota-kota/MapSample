package com.sample.appmain;

import android.graphics.PointF;

public class TouchPoint {
    public enum ActState {
        INIT,
        DOWN,
        UP,
        MOVE,
    }
    private ActState mState;    //タッチアクション状態
    private int mId;            //タッチID
    private PointF mLastPoint;  //最新タッチ座標
    private PointF mPrePoint;   //以前タッチ座標

    //コンストラクタ
    TouchPoint() {
        mState = ActState.INIT;
        mId = -1;
        mLastPoint = new PointF(0.0F, 0.0F);
        mPrePoint = new PointF(0.0F, 0.0F);
    }

    //タッチダウン
    boolean down(int id, float x, float y) {
        boolean ret = false;
        if ((mState == ActState.INIT) || (mState == ActState.UP)) {
            mState = ActState.DOWN;
            mId = id;
            mLastPoint.set(x, y);
            ret = true;
        }
        return ret;
    }

    //タッチアップ
    boolean up(int id, float x, float y) {
        boolean ret = false;
        if(mId == id) {
            mState = ActState.UP;
            mId = -1;
            mLastPoint.set(x, y);
            ret = true;
        }
        return ret;
    }

    //タッチ移動
    boolean move(float x, float y) {
        boolean ret = false;
        if((mState == ActState.DOWN) || (mState == ActState.MOVE)) {
            mState = ActState.MOVE;
            mPrePoint.set(mLastPoint.x, mLastPoint.y);
            mLastPoint.set(x, y);
            ret = true;
        }
        return ret;
    }

    //有効判定
    boolean enable() {
        boolean ret = true;
        if((mState == ActState.INIT) || (mState == ActState.UP)) {
            ret = false;
        }
        return ret;
    }

    //タッチIDを取得
    int getId() {
        return mId;
    }

    //最新タッチ座標を取得
    PointF getLastPoint() {
        return mLastPoint;
    }

    //一つ前のタッチ座標を取得
    PointF getPrePoint() {
        return mPrePoint;
    }
}
