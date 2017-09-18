#ifndef INCLUDED_UIMNG_HPP
#define INCLUDED_UIMNG_HPP

#include "FwStd.hpp"
#include "graphics/DrawIF.hpp"
#include "graphics/Bitmap.hpp"
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
		fw::DrawIF*	drawIF_;

		fw::CoordI32	mapPos_;
		fw::CoordI16	screenPos_;
		fw::CoordI16	touchPos_;
		fw::CoordI16	dragPos_;
		bool			isTouchOn_;
		bool			isDragOn_;

		fw::File		file_;
		std::uint32_t	texId_;

	public:
		//コンストラクタ
		UiMng(fw::DrawIF* drawIF = nullptr);
		//タッチON
		void setTouchOn(fw::CoordI16 touchPos);
		//タッチOFF
		void setTouchOff();
		//ドラッグ
		void setDrag(fw::CoordI16 dragPos);
		//描画
		void draw();
	};
}

#endif //INCLUDED_UIMNG
