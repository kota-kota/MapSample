package com.example.kyohei.androidwindow2;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;

public class MainFragment extends Fragment {

    private final static String LOG_TAG = "AndroidWindow";

    //MainFragmentのインスタンスを生成
    public static MainFragment newInstance() {
        Log.i(LOG_TAG, "MainFragment:newInstance");
        //インスタンスを生成
        MainFragment fragment = new MainFragment();

        //Fragmentに渡す値をBundleに格納してFragmentにセットする
        //TODO:渡す値があればこの仕組みを使う
        Bundle args = new Bundle();
        fragment.setArguments(args);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MainFragment:onCreate");
        super.onCreate(savedInstanceState);

        //Bundleを取得し、あれば値を受け取る
        Bundle args = getArguments();
        if (args != null) {
            //TODO:受け取る値があればここで受け取る
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MainFragment:onCreateView");
        //レイアウトをViewとして作成
        return inflater.inflate(R.layout.fragment_main, container, false);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MainFragment:onViewCreated");
        super.onViewCreated(view, savedInstanceState);

        //メニューボタン
        ImageButton buttonMenu = view.findViewById(R.id.btMenu);
        buttonMenu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(LOG_TAG, "MainFragment:onClick [MENU]");
            }
        });
    }
}
