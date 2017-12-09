#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "Std.hpp"
#include <thread>

namespace fw {
	//前方宣言
	class DrawIF;
	class LocalImage;
}

namespace ui {

	//----------------------------------------------------------
	//
	// UI管理クラス
	//
	//----------------------------------------------------------

	class UiMng {
		//メンバ変数
		std::thread		th_;
		bool			isStart_;
		fw::DrawIF*		drawIF_;
		fw::LocalImage*	localImage_;

		std::CoordI		mapCenterPos_;
		std::Area		mapArea_;
		std::CoordI		screenCenterPos_;
		std::CoordI		touchPos_;
		std::CoordI		dragPos_;
		bool			isTouchOn_;
		bool			isDragOn_;

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
		//タッチON
		void setTouchOn(std::CoordI touchPos);
		//タッチOFF
		void setTouchOff();
		//ドラッグ
		void setDrag(std::CoordI dragPos);
		//描画
		void draw();

	private:
		//メイン
		void main();
	};
}

#endif //INCLUDED_UIMNG
