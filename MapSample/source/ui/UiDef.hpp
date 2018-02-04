#ifndef INCLUDED_UIDEF_HPP
#define INCLUDED_UIDEF_HPP

namespace ui {

	//ボタンイベント
	enum EN_ButtonEvent {
		INVALID,		//イベントなし
		MOVE,			//ボタン移動
		LEFT_DOWN,		//左ボタン押す
		LEFT_UP,		//左ボタン離す
		LEFT_MOVE,		//左ボタン押下+移動
		WHEEL_FORWARD,	//ホイール奥
		WHEEL_BACKWARD,	//ホイール手前
		RIGHT_DOWN,		//右ボタン押す
		RIGHT_UP,		//右ボタン離す
		RIGHT_MOVE,		//右ボタン押下+移動
	};
}

#endif //INCLUDED_UIDEF_HPP
