#pragma once
#include "DrawIF.hpp"

namespace ui
{
	class UiMng {
		//メンバ変数
		draw::DrawIF*	drawIF;

	public:
		//コンストラクタ
		UiMng(draw::DrawIF* drawIF = nullptr);
		//描画
		void draw();
	};
}
