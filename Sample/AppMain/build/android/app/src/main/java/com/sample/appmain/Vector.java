package com.sample.appmain;

import android.graphics.PointF;

public class Vector {
    private PointF mPoint1;
    private PointF mPoint2;
    private float mX;
    private float mY;
    private float mLengthPow2;   //ベクトルの長さの2乗

    //コンストラクタ
    Vector(PointF point1, PointF point2) {
        mPoint1 = point1;
        mPoint2 = point2;
        mX = mPoint2.x - mPoint1.x;
        mY = mPoint2.y - mPoint1.y;
        mLengthPow2 = (mX * mX) + (mY * mY);
    }

    //ベクトルのXY座標を取得
    float getX() {
        return mX;
    }
    float getY() {
        return mY;
    }

    //ベクトルの長さを取得
    float getLength() {
        return (float)Math.sqrt(mLengthPow2);
    }
    float getLengthPow2() {
        return mLengthPow2;
    }

    //ベクトルの内積
    float dot(Vector v) {
        Vector v1 = this;
        Vector v2 = v;
        return (v1.getX() * v2.getX()) + (v1.getY() * v2.getY());
    }
}
