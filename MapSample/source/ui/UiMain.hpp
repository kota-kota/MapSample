#ifndef INCLUDED_UIMAIN_HPP
#define INCLUDED_UIMAIN_HPP

#include "Std.hpp"
#include "UiDef.hpp"


namespace fw {
	//前方宣言
	class DrawIF;
}
namespace ui {
	//前方宣言
	class UiMng;
}


namespace ui {

	//----------------------------------------------------------
	//
	// UIメインクラス
	//
	//----------------------------------------------------------

	class UiMain {
		//メンバ変数
		UiMng*		uiMng_;

	public:
		//コンストラクタ
		UiMain(fw::DrawIF* drawIF);
		//デストラクタ
		~UiMain();
		//ボタンイベント処理
		void procButtonEvent(const ui::EN_ButtonEvent buttonEvent, const std::CoordI buttonPos);
	};
}

#endif //INCLUDED_UIMAIN_HPP
