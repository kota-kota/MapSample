#pragma once
#include <vector>
#include "Common.hpp"

namespace draw
{
	class DrawSetup {
	public:
		cmn::CoordI16	mScreenPos;
		cmn::CoordI32	mMapPos;
	};

	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
		//セットアップ
		virtual void setup(DrawSetup& drawSetup) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const cmn::ColorU8& color) = 0;
		//点描画
		virtual void drawPoint(const cmn::ColorU8& color, const std::vector<cmn::CoordI32>& coord, const float size) = 0;
		//ライン描画
		virtual void drawLine(const cmn::ColorU8& color, const std::vector<cmn::CoordI32>& coord, const float width) = 0;

		//描画エリア取得
		virtual void getDrawArea(cmn::Area* area) = 0;
	};
}