#pragma once
#include <Windows.h>
#include <gl/GL.h>

#include "Common.hpp"
#include "DrawIF.hpp"


namespace draw
{
	class DrawWGL : public DrawIF {
		HWND	hWnd;
		HDC		hDC;
		HGLRC	hGLRC;

	public:
		//コンストラクタ
		DrawWGL(HWND hWnd = nullptr);
		//デストラクタ
		virtual ~DrawWGL();
		//セットアップ
		virtual void setup();
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const cmn::ColorU8& color);
		//点描画
		virtual void drawPoint(const cmn::PointI32& point);
	};
}
