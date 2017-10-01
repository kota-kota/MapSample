#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "Std.hpp"
#include "draw/DrawIF.hpp"
#include "image/Image.hpp"
#include "io/File.hpp"

namespace ui
{
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

		std::vector<std::uint32_t>	texId_;

	public:
		//コンストラクタ
		UiMng(fw::DrawIF* drawIF = nullptr);
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
