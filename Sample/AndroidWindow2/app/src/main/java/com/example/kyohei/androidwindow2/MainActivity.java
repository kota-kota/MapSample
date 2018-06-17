package com.example.kyohei.androidwindow2;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.Window;

public class MainActivity extends AppCompatActivity
        implements MainFragment.MainFragmentListener, MenuFragment.MenuFragmentListener {

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

        // 画面がはじめて作成された時、初期画面に遷移
        if(savedInstanceState == null) {
            onShowInitScreen();
        }
    }

    //MainFragmentからメニュー表示イベントが通知された時の処理
    @Override
    public void onClickShowMenuEvent() {
        //Fragmentを変更するため、Transactionを取得
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        //MenuFragmentを作成して追加
        transaction.setCustomAnimations(android.R.anim.slide_in_left, android.R.anim.slide_out_right);
        transaction.add(R.id.actMain, MenuFragment.newInstance());
        transaction.commit();
    }

    //MenuFragmentからメニュー非表示イベントが通知された時の処理
    @Override
    public void onClickHideMenuEvent(Fragment fragment) {
        //Fragmentを変更するため、Transactionを取得
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        transaction.setCustomAnimations(R.anim.slide_in_right, R.anim.slide_out_left);
        //指定Fragmentを削除
        transaction.remove(fragment);
        transaction.commit();
    }

    //初期画面表示
    public void onShowInitScreen() {
        //Fragmentを変更するため、Transactionを取得
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        //MainFragmentを作成して追加
        transaction.add(R.id.actMain, MainFragment.newInstance());
        transaction.commit();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
