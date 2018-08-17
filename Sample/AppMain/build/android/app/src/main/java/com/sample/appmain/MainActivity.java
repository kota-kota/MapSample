package com.sample.appmain;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private final static String LOG_TAG = "AppMain";
    AppMainView appMainView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MainActivity:onCreate");
        super.onCreate(savedInstanceState);

        //AppMainViewを生成
        appMainView = new AppMainView(this);
        appMainView.create();

        //ビューにセット
        setContentView(appMainView);

        //activity_mainレイアウトのViewをセット
        View view = this.getLayoutInflater().inflate(R.layout.activity_main, null);
        ViewGroup.LayoutParams param = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        addContentView(view, param);
    }

    @Override
    protected void onStart() {
        Log.i(LOG_TAG, "MainActivity:onStart");
        super.onStart();
    }

    @Override
    protected void onResume() {
        Log.i(LOG_TAG, "MainActivity:onResume");
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.i(LOG_TAG, "MainActivity:onPause");
        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.i(LOG_TAG, "MainActivity:onStop");
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.i(LOG_TAG, "MainActivity:onDestroy");
        super.onDestroy();

        //AppMainViewを破棄
        appMainView.destroy();
    }
}
