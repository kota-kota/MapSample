#include <Windows.h>
#include <cstdint>
#include <vector>

#include "DrawWGL.hpp"

#pragma comment(lib, "opengl32.lib")


//コンストラクタ
draw::DrawWGL::DrawWGL(HWND hWnd)
	: hWnd(hWnd), hDC(nullptr), hGLRC(nullptr)
{
	//デバイスコンテキストハンドルを取得
	this->hDC = ::GetDC(this->hWnd);

	//ピクセルフォーマット
	const PIXELFORMATDESCRIPTOR pFormat = {
		sizeof(PIXELFORMATDESCRIPTOR),	//nSize
		1,		//nVersion
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//dwFlags
		PFD_TYPE_RGBA,	//iPixelType
		32,		//cColorBits
		0,		//cRedBits
		0,		//cRedShift
		0,		//cGreenBits
		0,		//cGreenShift
		0,		//cBlueBits
		0,		//cBlueShift
		0,		//cAlphaBits
		0,		//cAlphaShift
		0,		//cAccumBits
		0,		//cAccumRedBits
		0,		//cAccumGreenBits
		0,		//cAccumBlueBits
		0,		//cAccumAlphaBits
		0,		//cDepthBits
		0,		//cStencilBits
		0,		//cAuxBuffers
		0,		//iLayerType
		0,		//bReserved
		0,		//dwLayerMask
		0,		//dwVisibleMask
		0		//dwDamageMask
	};

	//ピクセルフォーマットを選択
	std::int32_t format = ::ChoosePixelFormat(this->hDC, &pFormat);
	::SetPixelFormat(this->hDC, format, &pFormat);

	//描画コンテキストハンドルを作成
	this->hGLRC = ::wglCreateContext(this->hDC);
}

//デストラクタ
draw::DrawWGL::~DrawWGL()
{
	//描画コンテキストハンドルを破棄
	::wglDeleteContext(this->hGLRC);

	//デバイスコンテキストを破棄
	::ReleaseDC(this->hWnd, this->hDC);
}

//描画セットアップ
void draw::DrawWGL::setup()
{
	RECT rect;
	::GetClientRect(this->hWnd, &rect);
	std::int32_t width = rect.right - rect.left;
	std::int32_t height = rect.bottom - rect.top;

	//ビューポート設定
	glViewport(0, 0, width, height);
}

//描画カレント
void draw::DrawWGL::makeCurrent(const bool current)
{
	if (current) {
		//カレント設定
		::wglMakeCurrent(this->hDC, this->hGLRC);
	}
	else {
		//カレント解除
		::wglMakeCurrent(this->hDC, nullptr);
	}
}

//描画更新
void draw::DrawWGL::swapBuffers()
{
	::SwapBuffers(this->hDC);
}

//クリア
void draw::DrawWGL::clear(const cmn::ColorU8& color)
{
	GLclampf r = static_cast<GLclampf>(color.r) / 255.0f;
	GLclampf g = static_cast<GLclampf>(color.g) / 255.0f;
	GLclampf b = static_cast<GLclampf>(color.b) / 255.0f;
	GLclampf a = static_cast<GLclampf>(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

//点描画
void draw::DrawWGL::drawPoint(const cmn::PointI32& point)
{
}
