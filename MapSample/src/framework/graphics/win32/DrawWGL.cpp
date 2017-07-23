#include "DrawWGL.hpp"

//using指定
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::vector;
using cmn::Color;
using cmn::CoordI32;
using cmn::Size;
using graphics::DrawWGL;
using graphics::DrawSetup;

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
	//描画領域取得
	Size drawSize;
	this->getDrawSize(drawSize);

	//ビューポート設定
	glViewport(0, 0, drawSize.w, drawSize.h);

	//プロジェクション設定
	GLdouble left = drawSetup.mapPos_.x - drawSize.w / 2;
	GLdouble right = left + drawSize.w;
	GLdouble top = drawSetup.mapPos_.y - drawSize.h / 2;
	GLdouble bottom = top + drawSize.h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(left, right, bottom, top, -1, 1);
	glOrtho(left, right, top, bottom, -1, 1);

	//モデルビュー設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
void DrawWGL::clear(const Color& color)
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
void DrawWGL::drawPoint(const Color& color, const vector<CoordI32>& coord, const float size)
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
void DrawWGL::drawLine(const Color& color, const std::vector<CoordI32>& coord, const float width)
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

//テクスチャ描画
void DrawWGL::drawTextrue(const vector<CoordI32>& coord, const vector<uint8_t>& tex, const Size texSize)
{
	//テクスチャ作成
	GLuint texID;
	glGenTextures(1, &texID);

	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex[0]);

	//テクスチャ画像は1バイト単位
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//テクスチャパラメータ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//テクスチャ環境
	//テクスチャカラーを使用する
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//テクスチャ描画
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	{
		//色
		//念のためフラグメントカラーを1.0fで初期化する
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//座標
		auto itr = coord.begin();
		while (itr != coord.end()) {
			glTexCoord2i(itr->x, itr->y);
			itr++;
			glVertex3i(itr->x, itr->y, itr->z);
			itr++;
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texID);
}

//描画領域取得
void DrawWGL::getDrawSize(Size& drawSize)
{
	RECT rect;
	::GetClientRect(this->hWnd, &rect);
	drawSize.w = static_cast<int16_t>(rect.right - rect.left);
	drawSize.h = static_cast<int16_t>(rect.bottom - rect.top);
}
