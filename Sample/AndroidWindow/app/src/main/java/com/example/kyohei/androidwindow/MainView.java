package com.example.kyohei.androidwindow;

import android.content.Context;
import android.graphics.Canvas;
import android.support.constraint.ConstraintLayout;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainView
        extends ConstraintLayout
        implements GestureDetector.OnGestureListener, ScaleGestureDetector.OnScaleGestureListener, GestureDetector.OnDoubleTapListener {

    private final static String TAG = "AndroidWindow";

    private TextView tvDebug;
    private GestureDetector gesture;
    private ScaleGestureDetector scaleGesture;

    //コンストラクタ
    public MainView(Context context, AttributeSet attrs) {
        super(context, attrs);

        gesture = new GestureDetector(context, this);
        scaleGesture = new ScaleGestureDetector(context, this);

        // LayoutInflaterでレイアウトxmlの内容でViewを作る
        // LayoutInflater#inflate()の第2引数ではルートとなるViewとして自分自身を指定する
        View layout = LayoutInflater.from(context).inflate(R.layout.layout_main, this);

        //各コントロールを取り出す
        //TextView
        this.tvDebug = layout.findViewById(R.id.tvDebug);
        //SurfaceView
        SurfaceView sv = layout.findViewById(R.id.svSample);
        SurfaceView sampleSurfaceView = new SampleSurfaceView(context, sv);
        //Button
        Button btScaleDown = layout.findViewById(R.id.btScaleDown);
        Button btScaleUp = layout.findViewById(R.id.btScaleUp);

        //SurfaceViewクリック時の処理
        this.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                return (event.getPointerCount() == 1) ? gesture.onTouchEvent(event) : scaleGesture.onTouchEvent(event);
            }
       });

        //[+]釦クリック時の処理
        btScaleDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String log = "Press Button [＋]";
                tvDebug.setText(log);
                Log.i(TAG, log);
            }
        });

        //[-]釦クリック時の処理
        btScaleUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String log = "Press Button [－]";
                tvDebug.setText(log);
                Log.i(TAG, log);
            }
        });
    }

    //レイアウト描画
    @Override
    protected void dispatchDraw(Canvas canvas) {
        Log.i(TAG, "dispatchDraw");
        super.dispatchDraw(canvas);
    }

    //指が画面に押された GestureDetector.OnGestureListener
    public boolean onDown(MotionEvent e) {
        float x = e.getX();
        float y = e.getY();
        String log = "onDown x:" + x + " y:" + y;
        tvDebug.setText(log);
        Log.i(TAG, log);
        return true;
    }

    //指が画面を離れた GestureDetector.OnGestureListener
    public boolean onSingleTapUp(MotionEvent e) {
        float x = e.getX();
        float y = e.getY();
        String log = "onSingleTapUp x:" + x + " y:" + y;
        tvDebug.setText(log);
        Log.i(TAG, log);

        Jni.nativeDraw();
        invalidate();

        return true;
    }

    //画面を少し長押しした(使用しない) GestureDetector.OnGestureListener
    public void onShowPress(MotionEvent e) {
        //String log = "onShowPress";
        //tvDebug.setText(log);
    }

    //画面を長押しした GestureDetector.OnGestureListener
    public void onLongPress(MotionEvent e) {
        String log = "onLongPress";
        tvDebug.setText(log);
        Log.i(TAG, log);
    }

    //指が画面上を移動した GestureDetector.OnGestureListener
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        float x = e2.getX();
        float y = e2.getY();
        String log = "onScroll x:" + x + " y:" + y;
        //tvDebug.setText(log);
        Log.i(TAG, log);
        return true;
    }

    //指が画面上をさっとなぞった(フリックした) GestureDetector.OnGestureListener
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        String log = "onFling";
        tvDebug.setText(log);
        Log.i(TAG, log);
        return true;
    }

    //ピンチイン/アウト開始　ScaleGestureDetector.OnScaleGestureListener
    public boolean onScaleBegin(ScaleGestureDetector detector) {
        String log = "onScaleBegin";
        //tvDebug.setText(log);
        Log.i(TAG, log);
        invalidate();
        return true;
    }

    //ピンチイン/アウト終了　ScaleGestureDetector.OnScaleGestureListener
    public void onScaleEnd(ScaleGestureDetector detector) {
        String log = "onScaleEnd";
        tvDebug.setText(log);
        Log.i(TAG, log);
        invalidate();
    }

    //ピンチイン/アウト　ScaleGestureDetector.OnScaleGestureListener
    public boolean onScale(ScaleGestureDetector detector) {
        float scaleFactor = detector.getScaleFactor();
        String log = "onScale scale:" + scaleFactor;
        //tvDebug.setText(log);
        Log.i(TAG, log);
        invalidate();
        return true;
    }

    //指が画面をダブルタップした GestureDetector.OnDoubleTapListener
    public boolean onDoubleTap(MotionEvent e) {
        float x = e.getX();
        float y = e.getY();
        String log = "onDoubleTap x:" + x + " y:" + y;
        tvDebug.setText(log);
        Log.i(TAG, log);
        return true;
    }

    //ダブルタップ中のイベント(押す、スクロール、離す) GestureDetector.OnDoubleTapListener
    public boolean onDoubleTapEvent(MotionEvent e) {
        return true;
    }

    //指が画面をシングルタップした GestureDetector.OnDoubleTapListener
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return true;
    }
}