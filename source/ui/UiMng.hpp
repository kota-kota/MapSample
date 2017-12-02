#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "Std.hpp"

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
		fw::DrawIF*		drawIF_;

		std::CoordI		mapCenterPos_;
		std::Area		mapArea_;
		std::CoordI		screenCenterPos_;
		std::CoordI		touchPos_;
		std::CoordI		dragPos_;
		bool			isTouchOn_;
		bool			isDragOn_;

		fw::LocalImage*	localImage;

	public:
		//コンストラクタ
		UiMng(fw::DrawIF* drawIF = nullptr);
		//デストラクタ
		~UiMng();
		//タッチON
		void setTouchOn(std::CoordI touchPos);
		//タッチOFF
		void setTouchOff();
		//ドラッグ
		void setDrag(std::CoordI dragPos);
		//描画
		void draw();
	};
}

#endif //INCLUDED_UIMNG
