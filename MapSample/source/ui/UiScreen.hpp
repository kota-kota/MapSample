#ifndef INCLUDED_UISCREEN_HPP
#define INCLUDED_UISCREEN_HPP

#include "Std.hpp"
#include "UiDef.hpp"
#include "ViewData.hpp"

namespace fw {
	//前方宣言
	class DrawIF;
	class LocalImage;
}

namespace ui {

	//----------------------------------------------------------
	//
	// UIステータス構造体
	//
	//----------------------------------------------------------

	struct UiStatus {
		std::EN_OffOn	isUpdate_;
		fw::DrawIF*		drawIF_;
		fw::LocalImage*	localImage_;
		std::Position	screenPos_;
		std::Position	mapPos_;
		std::AreaI		mapArea_;
	};


	//----------------------------------------------------------
	//
	// UI画面クラス
	//
	//----------------------------------------------------------

	class UiScreen {
		//メンバ変数
		UiStatus			status_;
		ViewData			viewData_;

		std::Position		touchPos_;
		std::Position		dragPos_;
		ui::EN_ButtonEvent	buttonEvent_;

	public:
		//コンストラクタ
		UiScreen(fw::DrawIF* const drawIF, fw::LocalImage* const localImage, const std::Position& mapPos);
		//デストラクタ
		~UiScreen();
		//表示データ作成
		void makeViewData();
		//描画更新必要有無
		bool isUpdateDraw();
		//描画
		void draw();
		//ボタンイベント処理:LEFT_DOWN
		void procButtonLeftDown(const std::Position& buttonPos);
		//ボタンイベント処理:LEFT_UP
		void procButtonLeftUp();
		//ボタンイベント処理:MOVE
		void procButtonMove(const std::Position& buttonPos);
	};
}

#endif //INCLUDED_UISCREEN_HPP
