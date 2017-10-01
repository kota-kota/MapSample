#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "DrawIF.hpp"

namespace fw
{
	//----------------------------------------------------------
	//
	// WGL描画クラス
	//
	//----------------------------------------------------------

	class DrawWGL : public DrawIF {
		class Impl;
		Impl*	impl_;

	public:
		//コンストラクタ
		DrawWGL();
		//デストラクタ
		virtual ~DrawWGL();
		//作成
		virtual void create(void* const hWnd);
		//セットアップ
		virtual void setup(const std::CoordI mapPos);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const std::Color color);
		//点描画
		virtual void drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size);
		//ライン描画
		virtual void drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width);
		//テクスチャ作成
		virtual void createTextures(const fw::Image* const tex, std::uint32_t* const texId);
		//テクスチャ削除
		virtual void deleteTextures(const std::uint32_t texId);
		//テクスチャ描画
		virtual void drawTextrue(const std::vector<std::CoordTex>& coords, const std::uint32_t texId);

		//画面サイズ取得
		virtual void getScreenSize(std::Size* const screenSize);
	};
}

#endif //INCLUDED_DRAWWGL_HPP
