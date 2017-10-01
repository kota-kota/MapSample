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
	class Image;

	//----------------------------------------------------------
	//
	// 描画インターフェースクラス
	//
	//----------------------------------------------------------

	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
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
		//テクスチャ作成
		virtual void createTextures(const fw::Image* const tex, std::uint32_t* const texId) = 0;
		//テクスチャ削除
		virtual void deleteTextures(const std::uint32_t texId) = 0;
		//テクスチャ描画
		virtual void drawTextrue(const std::vector<std::CoordTex>& coords, const std::uint32_t texId) = 0;

		//画面サイズ取得
		virtual void getScreenSize(std::Size* const screenSize) = 0;
	};
}

#endif //INCLUDED_DRAWIF_HPP
