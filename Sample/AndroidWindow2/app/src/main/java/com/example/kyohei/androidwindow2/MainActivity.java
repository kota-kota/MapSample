package com.example.kyohei.androidwindow2;

import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private final static String LOG_TAG = "AndroidWindow";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MainActivity:onCreate");
        super.onCreate(savedInstanceState);

        //タイトルバーを非表示にする
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.activity_main);

        // 画面がはじめて作成された時ににだけ、Fragmentを追加する
        if(savedInstanceState == null) {
            //Fragmentを変更するため、Transactionを取得
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();

            //MainFragmentを作成して追加
            transaction.add(R.id.actMain, MainFragment.newInstance());
            transaction.commit();
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
