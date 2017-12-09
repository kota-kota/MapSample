#ifndef INCLUDED_UIMAIN_HPP
#define INCLUDED_UIMAIN_HPP

#include "Std.hpp"

namespace fw {
	//前方宣言
	class DrawIF;
}

namespace ui {

	//前方宣言
	class UiMng;

	//----------------------------------------------------------
	//
	// UIメインクラス
	//
	//----------------------------------------------------------

	class UiMain {
		//メンバ変数
		fw::DrawIF*	drawIF_;
		UiMng*		uiMng_;

	public:
		//コンストラクタ
		UiMain(fw::DrawIF* drawIF);
		//デストラクタ
		~UiMain();
		//タッチON
		void touchOn(std::CoordI touchPos);
		//タッチOFF
		void touchOff();
		//タッチ移動
		void touchMove(std::CoordI dragPos);
		//描画
		void draw();
	};
}

#endif //INCLUDED_UIMAIN_HPP
