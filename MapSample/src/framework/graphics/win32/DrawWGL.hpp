#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "FwStd.hpp"
#include "../DrawIF.hpp"
#include <Windows.h>

namespace fw
{
	//----------------------------------------------------------
	//
	// WGL描画クラス
	//
	//----------------------------------------------------------

	class DrawWGL : public DrawIF {
		HWND	hWnd_;
		HDC		hDC_;
		HGLRC	hGLRC_;

	public:
		//コンストラクタ
		DrawWGL(HWND hWnd = nullptr);
		//デストラクタ
		virtual ~DrawWGL();
		//セットアップ
		virtual void setup(CoordI32 mapPos);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const Color color);
		//点描画
		virtual void drawPoint(const ACoordI32& coords, const AColor& colors, const std::float32_t size);
		//ライン描画
		virtual void drawLine(const ACoordI32& coords, const AColor& colors, const std::float32_t width);
		//テクスチャ作成
		virtual void createTexture(const AImage& tex, const Size texSize, std::uint32_t& texId);
		//テクスチャ削除
		virtual void deleteTexture(const std::uint32_t texId);
		//テクスチャ描画
		virtual void drawTextrue(const ACoordI32& coords, const std::uint32_t texId);

		//描画領域取得
		virtual void getDrawSize(Size& drawSize);
	};
}

#endif //INCLUDED_DRAWWGL_HPP
