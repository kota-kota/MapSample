package com.sample.appmain;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MapSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private final static String LOG_TAG = "AppMain";
    //タッチポインタID記憶用
    private int pointerId1 = -1;
    private int pointerId2 = -1;
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

    @Override
    public boolean performClick() {
        super.performClick();
        return true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        performClick();
        final int action = event.getActionMasked();
        final int pointerIndex = event.getActionIndex();
        final int pointerId = event.getPointerId(pointerIndex);
        switch (action) {
            case MotionEvent.ACTION_DOWN: {
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                pointerId1 = pointerId;
                pointerId2 = -1;
                Log.i(LOG_TAG, "ACTION_DOWN id:" + pointerId + " (" + x + "," + y + ")");
                break;
            }
            case MotionEvent.ACTION_POINTER_DOWN: {
                if (pointerId2 == -1) {
                    float x = event.getX(pointerIndex);
                    float y = event.getY(pointerIndex);
                    pointerId2 = pointerId;
                    Log.i(LOG_TAG, "ACTION_POINTER_DOWN id:" + pointerId + " (" + x + "," + y + ")");
                } else if (pointerId1 == -1) {
                    float x = event.getX(pointerIndex);
                    float y = event.getY(pointerIndex);
                    pointerId1 = pointerId;
                    Log.i(LOG_TAG, "ACTION_POINTER_DOWN id:" + pointerId + " (" + x + "," + y + ")");
                }
                break;
            }
            case MotionEvent.ACTION_MOVE: {
                float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
                if (pointerId1 >= 0) {
                    final int i = event.findPointerIndex(pointerId1);
                    x1 = event.getX(i);
                    y1 = event.getY(i);
                }
                if (pointerId2 >= 0) {
                    final int i = event.findPointerIndex(pointerId2);
                    x2 = event.getX(i);
                    y2 = event.getY(i);
                }

                // ジェスチャー処理
                if ((pointerId1 >= 0) && (pointerId2 == -1)) {
                    Log.i(LOG_TAG, "ACTION_MOVE<1> (" + x1 + "," + y1 + ")");
                } else if ((pointerId1 == -1) && (pointerId2 >= 0)) {
                    Log.i(LOG_TAG, "ACTION_MOVE<2>  (" + x2 + "," + y2 + ")");
                } else if ((pointerId1 >= 0) && (pointerId2 >= 0)) {
                    Log.i(LOG_TAG, "ACTION_MOVE<1,2> (" + x1 + "," + y1 + ") (" + x2 + "," + y2 + ")");
                }
                break;
            }
            case MotionEvent.ACTION_UP:
                pointerId1 = -1;
                pointerId2 = -1;
                Log.i(LOG_TAG, "ACTION_UP id:" + pointerId);
                break;
            case MotionEvent.ACTION_POINTER_UP: {
                if (pointerId1 == pointerId) {
                    pointerId1 = -1;
                } else if (pointerId2 == pointerId) {
                    pointerId2 = -1;
                }
                Log.i(LOG_TAG, "ACTION_POINTER_UP id:" + pointerId);
                break;
            }
            case MotionEvent.ACTION_CANCEL:
                pointerId1 = -1;
                pointerId2 = -1;
                Log.i(LOG_TAG, "ACTION_CANCEL");
                break;

            default:
                break;
        }
        return true;
    }
}
