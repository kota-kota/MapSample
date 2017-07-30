#ifndef INCLUDED_UIMNG
#define INCLUDED_UIMNG

#include "Common.hpp"
#include "graphics/DrawIF.hpp"
#include "graphics/Bitmap.hpp"
#include "io/File.hpp"

namespace ui
{
	class UiMng {
		//メンバ変数
		graphics::DrawIF*	drawIF_;

		cmn::CoordI32		mapPos_;
		cmn::CoordI16		screenPos_;
		cmn::CoordI16		touchPos_;
		cmn::CoordI16		dragPos_;
		bool				isTouchOn_;
		bool				isDragOn_;

		io::File			file_;
		graphics::Bitmap	bitmap_;

	public:
		//コンストラクタ
		UiMng(graphics::DrawIF* drawIF = nullptr);
		//タッチON
		void setTouchOn(cmn::CoordI16 touchPos);
		//タッチOFF
		void setTouchOff();
		//ドラッグ
		void setDrag(cmn::CoordI16 dragPos);
		//描画
		void draw();
	};
}

#endif //INCLUDED_UIMNG
