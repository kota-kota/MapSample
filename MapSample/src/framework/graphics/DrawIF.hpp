#ifndef INCLUDED_DRAWIF
#define INCLUDED_DRAWIF

#include "Common.hpp"
#include <vector>

namespace graphics
{
	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
		//セットアップ
		virtual void setup(cmn::CoordI32 mapPos) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const cmn::Color& color) = 0;
		//点描画
		virtual void drawPoint(const cmn::Color& color, const std::vector<cmn::CoordI32>& coord, const float size) = 0;
		//ライン描画
		virtual void drawLine(const cmn::Color& color, const std::vector<cmn::CoordI32>& coord, const float width) = 0;
		//テクスチャ描画
		virtual void drawTextrue(const std::vector<cmn::CoordI32>& coord, const std::vector<std::uint8_t>& tex, const cmn::Size texSize) = 0;

		//描画領域取得
		virtual void getDrawSize(cmn::Size& drawSize) = 0;
	};
}

#endif //INCLUDED_DRAWIF
