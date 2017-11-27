#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "Std.hpp"

namespace fw {
	class DrawIF;
}

namespace ui {
	//前方宣言
	class LocalImage;

	//----------------------------------------------------------
	//
	// UI管理クラス
	//
	//----------------------------------------------------------

	class UiMng {
		//メンバ変数
		fw::DrawIF*		drawIF_;

		std::CoordI		mapPos_;
		std::CoordI		screenPos_;
		std::CoordI		touchPos_;
		std::CoordI		dragPos_;
		bool			isTouchOn_;
		bool			isDragOn_;

		std::CoordI		texBasePos_;
		LocalImage*		localImage;

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
