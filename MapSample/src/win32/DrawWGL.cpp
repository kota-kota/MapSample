#include <Windows.h>
#include <cstdint>
#include <vector>
#include "win32/DrawWGL.hpp"

#pragma comment(lib, "opengl32.lib")

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::vector;
using cmn::ColorU8;
using cmn::CoordI32;
using cmn::Area;
using draw::DrawWGL;
using draw::DrawSetup;

//コンストラクタ
DrawWGL::DrawWGL(HWND hWnd)
	: hWnd(hWnd), hDC(nullptr), hGLRC(nullptr)
{
	if (this->hWnd != nullptr) {
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
			24,		//cDepthBits
			8,		//cStencilBits
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
}

//デストラクタ
DrawWGL::~DrawWGL()
{
	//描画コンテキストハンドルを破棄
	::wglDeleteContext(this->hGLRC);

	//デバイスコンテキストを破棄
	::ReleaseDC(this->hWnd, this->hDC);
}

//描画セットアップ
void DrawWGL::setup(DrawSetup& drawSetup)
{
	//描画エリア取得
	Area area;
	this->getDrawArea(&area);

	//ビューポート設定
	glViewport(0, 0, area.w, area.h);

	//プロジェクション設定
	double left = drawSetup.mMapPos.x - area.w / 2;
	double right = left + area.w;
	double top = drawSetup.mMapPos.y - area.h / 2;
	double bottom = top + area.h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -1, 1);
}

//描画カレント
void DrawWGL::makeCurrent(const bool current)
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
void DrawWGL::swapBuffers()
{
	::SwapBuffers(this->hDC);
}

//クリア
void DrawWGL::clear(const cmn::ColorU8& color)
{
	GLclampf r = static_cast<GLclampf>(color.r) / 255.0f;
	GLclampf g = static_cast<GLclampf>(color.g) / 255.0f;
	GLclampf b = static_cast<GLclampf>(color.b) / 255.0f;
	GLclampf a = static_cast<GLclampf>(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//点描画
void DrawWGL::drawPoint(const ColorU8& color, const vector<CoordI32>& coord, const float size)
{
	GLfloat r = static_cast<GLfloat>(color.r) / 255.0f;
	GLfloat g = static_cast<GLfloat>(color.g) / 255.0f;
	GLfloat b = static_cast<GLfloat>(color.b) / 255.0f;
	GLfloat a = static_cast<GLfloat>(color.a) / 255.0f;

	//点の大きさを設定
	glPointSize(size);

	//点描画
	glBegin(GL_POINTS);
	{
		//色
		glColor4f(r, g, b, a);

		//座標
		for (auto itr = coord.begin(); itr != coord.end(); itr++) {
			glVertex3i(itr->x, itr->y, itr->z);
		}
	}
	glEnd();
}

//ライン描画
void DrawWGL::drawLine(const cmn::ColorU8& color, const std::vector<cmn::CoordI32>& coord, const float width)
{
	GLfloat r = static_cast<GLfloat>(color.r) / 255.0f;
	GLfloat g = static_cast<GLfloat>(color.g) / 255.0f;
	GLfloat b = static_cast<GLfloat>(color.b) / 255.0f;
	GLfloat a = static_cast<GLfloat>(color.a) / 255.0f;

	//ラインの太さを設定
	glLineWidth(width);

	//ライン描画
	glBegin(GL_LINE_LOOP);
	{
		//色
		glColor4f(r, g, b, a);

		//座標
		for (auto itr = coord.begin(); itr != coord.end(); itr++) {
			glVertex3i(itr->x, itr->y, itr->z);
		}
	}
	glEnd();
}

//描画エリア取得
void DrawWGL::getDrawArea(Area* area)
{
	RECT rect;
	::GetClientRect(this->hWnd, &rect);
	area->w = static_cast<int16_t>(rect.right - rect.left);
	area->h = static_cast<int16_t>(rect.bottom - rect.top);
}
