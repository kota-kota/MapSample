package com.example.kyohei.androidwindow2;

import android.graphics.Point;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.ViewGroup;

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

        //ナビゲーションバー非表示、フルスクリーン
        int visibility = 0;
        visibility |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;  //ナビゲーションバー非表示
        visibility |= View.SYSTEM_UI_FLAG_FULLSCREEN;        //フルスクリーン
        visibility |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY; //システムUI表示後も自動で戻る
        View decor = getWindow().getDecorView();
        decor.setSystemUiVisibility(visibility);

        //NativeViewをセット
        setContentView(new NativeView(this));

        //activity_mainレイアウトのViewをセット
        View view = this.getLayoutInflater().inflate(R.layout.activity_main, null);
        ViewGroup.LayoutParams param = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        addContentView(view, param);

        // 画面がはじめて作成された時、初期画面に遷移
        if(savedInstanceState == null) {
            Fragment fragment;

            //Fragmentを変更するため、Transactionを取得
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();

            //MainFragmentを作成して追加
            fragment = MainFragment.newInstance();
            transaction.replace(R.id.actMain, fragment, MainFragment.TAG);

            //MenuFragmentを作成して追加して非表示にしておく
            fragment = MenuFragment.newInstance();
            transaction.add(R.id.actMain, fragment, MenuFragment.TAG);
            transaction.hide(fragment);

            //Fragmentの変更を反映
            transaction.commit();
        }
    }

    @Override
    protected void onStart() {
        Log.i(LOG_TAG, "MainActivity:onStart");
        super.onStart();

        nativeStart();
    }

    //MainFragmentからメニュー表示イベントが通知された時の処理
    @Override
    public void onClickShowMenuEvent() {
        Log.i(LOG_TAG, "MainActivity:onClickShowMenuEvent");
        //Fragmentを変更するため、Transactionを取得
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        //アニメーションを設定
        transaction.setCustomAnimations(android.R.anim.slide_in_left, android.R.anim.slide_out_right);
        //MenuFragmentを表示
        Fragment fragment = getSupportFragmentManager().findFragmentByTag(MenuFragment.TAG);
        transaction.show(fragment);
        transaction.commit();
    }

    //MenuFragmentからメニュー非表示イベントが通知された時の処理
    @Override
    public void onClickHideMenuEvent() {
        Log.i(LOG_TAG, "MainActivity:onClickHideMenuEvent");
        //Fragmentを変更するため、Transactionを取得
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        //アニメーションを設定
        transaction.setCustomAnimations(R.anim.slide_in_right, R.anim.slide_out_left);
        //MenuFragmentを非表示
        Fragment fragment = getSupportFragmentManager().findFragmentByTag(MenuFragment.TAG);
        transaction.hide(fragment);
        transaction.commit();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void nativeStart();
}
