#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "Std.hpp"
#include "UiDef.hpp"
#include <thread>

namespace fw {
	//前方宣言
	class DrawIF;
	class LocalImage;
}
namespace ui {
	//前方宣言
	class UiScreen;
}

namespace ui {

	//----------------------------------------------------------
	//
	// UI管理クラス
	//
	//----------------------------------------------------------

	class UiMng {
		//メンバ変数
		std::EN_OffOn	isStart_;
		std::thread		mainThread_;
		std::thread		drawThread_;

		fw::DrawIF*		drawIF_;
		fw::LocalImage*	localImage_;
		UiScreen*		screen_;

	public:
		//コンストラクタ
		UiMng(fw::DrawIF* drawIF);
		//デストラクタ
		~UiMng();
		//タスク開始有無
		bool isStart();
		//タスク開始
		void start();
		//タスク終了
		void end();
		//ボタンイベント処理
		void procButtonEvent(const ui::EN_ButtonEvent buttonEvent, const std::CoordI buttonPos);
	};
}

#endif //INCLUDED_UIMNG
