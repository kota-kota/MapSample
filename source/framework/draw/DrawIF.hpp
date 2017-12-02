#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "Std.hpp"
#include <vector>

namespace fw
{
	//----------------------------------------------------------
	//
	// 前方宣言
	//
	//----------------------------------------------------------
	struct Image;

	//----------------------------------------------------------
	//
	// 描画インターフェースクラス
	//
	//----------------------------------------------------------

	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
		//作成
		virtual void create() = 0;
		//セットアップ
		virtual void setup(const std::CoordI mapPos) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const std::Color color) = 0;
		//点描画
		virtual void drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size) = 0;
		//ライン描画
		virtual void drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width) = 0;
		//イメージ描画
		virtual void drawImage(const std::CoordI coord, const fw::Image& image) = 0;

		//画面幅高さを取得
		virtual std::WH getScreenWH() = 0;
	};
}

#endif //INCLUDED_DRAWIF_HPP
