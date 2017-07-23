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
		graphics::DrawSetup	drawSetup_;

		io::File			file_;
		graphics::Bitmap	bitmap_;

	public:
		//コンストラクタ
		UiMng(graphics::DrawIF* drawIF = nullptr);
		//画面中心位置を設定
		void setScreenPosition(cmn::CoordI16 screenPos);
		//描画
		void draw();
	};
}

#endif //INCLUDED_UIMNG
