#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "DrawIF.hpp"

#ifdef DRAWIF_WGL
#include <Windows.h>

namespace image {
	//前方宣言
	class Font;
}

namespace fw {

	//----------------------------------------------------------
	//
	// WGL描画クラス
	//
	//----------------------------------------------------------

	class DrawWGL : public DrawIF {
		HWND			hWnd_;
		HDC				hDC_;
		HGLRC			hGLRC_;

	public:
		//コンストラクタ
		DrawWGL(const HWND hWnd);
		//デストラクタ
		virtual ~DrawWGL();
		//作成
		virtual void create();
		//セットアップ
		virtual void setup(const DrawStatus& drawStatus);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const std::ColorUB& color);
		//点描画
		virtual void drawPoints(const DrawCoords& coords, const DrawColors& colors, const std::float_t size);
		//ライン描画
		virtual void drawLines(const DrawCoords& coords, const DrawColors& colors, const std::float_t width);
		//ポリゴン描画
		virtual void drawPolygons(const DrawCoords& coords, const DrawColors& colors);
		//イメージ描画
		virtual void drawImage(const std::CoordI& coord, const fw::Image& image);
		//文字描画
		virtual void drawString(const std::CoordI& coord, const wchar_t* const str);

		//画面幅高さを取得
		virtual std::WH getScreenWH();
	};
}

#endif //DRAWIF_WGL
#endif //INCLUDED_DRAWWGL_HPP
