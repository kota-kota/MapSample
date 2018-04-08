#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "Type.hpp"

namespace draw {

	struct PointF {
		std::float32_t	x_;
		std::float32_t	y_;
	};

	struct ColorUB {
		std::uint8_t	r_;
		std::uint8_t	g_;
		std::uint8_t	b_;
		std::uint8_t	a_;
	};
	
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
		virtual void clear(const ColorUB& color) = 0;
		//ライン描画
		virtual void drawLines(const ColorUB& color, PointF* const points, const int32_t pointNum, const std::float32_t width) = 0;
	};
}

#endif //INCLUDED_DRAWIF_HPP
