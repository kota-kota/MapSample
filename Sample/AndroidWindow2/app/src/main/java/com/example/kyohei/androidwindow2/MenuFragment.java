package com.example.kyohei.androidwindow2;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;

public class MenuFragment extends Fragment {

    private final static String LOG_TAG = "AndroidWindow";

    private MainActivity myParent;
    private MenuFragment myFragment;

    //コンストラクタ
    public MenuFragment() {
        //何もしない
    }

    //MenuFragment
    public static MenuFragment newInstance() {
        Log.i(LOG_TAG, "MenuFragment:newInstance");
        //インスタンスを生成
        MenuFragment fragment = new MenuFragment();

        //Fragmentに渡す値をBundleに格納してFragmentにセットする
        //TODO:渡す値があればこの仕組みを使う
        Bundle args = new Bundle();
        fragment.setArguments(args);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MenuFragment:onCreate");
        super.onCreate(savedInstanceState);

        //Bundleを取得し、あれば値を受け取る
        Bundle args = getArguments();
        if (args != null) {
            //TODO:受け取る値があればここで受け取る
        }

        //親Activityを取得
        myParent = (MainActivity)getActivity();
        //自身を保持
        myFragment = this;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MenuFragment:onCreateView");
        //レイアウトをViewとして作成
        return inflater.inflate(R.layout.fragment_menu, container, false);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        Log.i(LOG_TAG, "MenuFragment:onViewCreated");
        super.onViewCreated(view, savedInstanceState);

        //戻るボタン
        ImageButton buttonBack = view.findViewById(R.id.btBack);
        buttonBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(LOG_TAG, "MenuFragment:onClick [BACK]");

                //非表示にする
                myParent.onHide(myFragment);
            }
        });
    }
}
