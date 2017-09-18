#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "FwStd.hpp"

namespace fw
{
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
		virtual void setup(CoordI32 mapPos) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const Color color) = 0;
		//点描画
		virtual void drawPoint(const ACoordI32& coords, const AColor& colors, const std::float32_t size) = 0;
		//ライン描画
		virtual void drawLine(const ACoordI32& coords, const AColor& colors, const std::float32_t width) = 0;
		//テクスチャ作成
		virtual void createTexture(const AImage& tex, const Size texSize, std::uint32_t& texId) = 0;
		//テクスチャ削除
		virtual void deleteTexture(const std::uint32_t texId) = 0;
		//テクスチャ描画
		virtual void drawTextrue(const ACoordI32& coords, const std::uint32_t texId) = 0;

		//描画領域取得
		virtual void getDrawSize(Size& drawSize) = 0;
	};
}

#endif //INCLUDED_DRAWIF_HPP
