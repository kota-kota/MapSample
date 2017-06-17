#pragma once
#include "Common.hpp"

namespace draw
{
	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
		//セットアップ
		virtual void setup() = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const cmn::ColorU8& color) = 0;
		//点描画
		virtual void drawPoint(const cmn::PointI32& point) = 0;
	};
}