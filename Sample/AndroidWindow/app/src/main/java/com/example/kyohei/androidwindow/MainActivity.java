package com.example.kyohei.androidwindow;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    private TextView tvDebug;
    private SurfaceView svSample;

    private int mPointerID1, mPointerID2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //各コントロールを取り出す
        //TextView
        this.tvDebug = findViewById(R.id.tvDebug);
        //SurfaceView
        this.svSample = findViewById(R.id.svSample);
        //Button
        Button btScaleDown = findViewById(R.id.btScaleDown);
        Button btScaleUp = findViewById(R.id.btScaleUp);

        //SurfaceViewのコールバック登録
        SurfaceHolder holder = this.svSample.getHolder();
        holder.addCallback(this);

        //SurfaceViewクリック時の処理
        this.svSample.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int eventAction = event.getAction();
                int pointerIndex = event.getActionIndex();
                int pointerId = event.getPointerId(pointerIndex);
                switch(eventAction) {
                    case MotionEvent.ACTION_DOWN:
                        mPointerID1 = pointerId;
                        mPointerID2 = -1;
                        tvDebug.setText("Touch ACTION_DOWN");
                        break;
                    case MotionEvent.ACTION_POINTER_DOWN:
                        break;
                    case MotionEvent.ACTION_UP:
                        tvDebug.setText("Touch ACTION_UP");
                        break;
                    case MotionEvent.ACTION_POINTER_UP:
                        break;
                    case MotionEvent.ACTION_MOVE:
                        float x1 = 0.0f;
                        float y1 = 0.0f;
                        if(mPointerID1 >= 0) {
                            int ptrIndex = event.findPointerIndex(mPointerID1);
                            x1 = event.getX(ptrIndex);
                            y1 = event.getY(ptrIndex);
                        }
                        if((mPointerID1 >= 0) && (mPointerID2 == -1)) {
                            //1本目の指だけが動いている
                            tvDebug.setText(String.format("Touch1: %3.1f, %3.1f", x1, y1));
                        }
                        break;
                    case MotionEvent.ACTION_CANCEL:
                        break;
                    default:
                        break;
                }
                return true;
            }
        });

        //[+]釦クリック時の処理
        btScaleDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                tvDebug.setText("Press Button [＋]");
            }
        });

        //[-]釦クリック時の処理
        btScaleUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                tvDebug.setText("Press Button [－]");
            }
        });
    }

    //Surface作成時
    public void surfaceCreated(SurfaceHolder holder) {
        nativeSurfaceCreated(holder.getSurface());
    }

    //Surface解放時
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    //Surface変化時
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void nativeSurfaceCreated(Surface surface);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
