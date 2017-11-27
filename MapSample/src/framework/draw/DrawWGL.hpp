#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "DrawIF.hpp"
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
		DrawWGL(const HWND hWnd = nullptr);
		//デストラクタ
		virtual ~DrawWGL();
		//作成
		virtual void create();
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
		//イメージ描画
		virtual void drawImage(const std::CoordI coord, const fw::Image& image);

		//画面幅高さを取得
		virtual std::WH getScreenWH();
	};
}

#endif //INCLUDED_DRAWWGL_HPP
