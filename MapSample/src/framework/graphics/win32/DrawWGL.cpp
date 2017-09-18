#include "DrawWGL.hpp"
#include <gl/GL.h>


//----------------------------------------------------------
//
// WGL描画クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawWGL::DrawWGL(HWND hWnd)
	: hWnd_(hWnd), hDC_(nullptr), hGLRC_(nullptr)
{
	if (this->hWnd_ != nullptr) {
		//デバイスコンテキストハンドルを取得
		this->hDC_ = ::GetDC(this->hWnd_);

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
		std::int32_t format = ::ChoosePixelFormat(this->hDC_, &pFormat);
		::SetPixelFormat(this->hDC_, format, &pFormat);

		//描画コンテキストハンドルを作成
		this->hGLRC_ = ::wglCreateContext(this->hDC_);
	}
}

//デストラクタ
fw::DrawWGL::~DrawWGL()
{
	//描画コンテキストハンドルを破棄
	::wglDeleteContext(this->hGLRC_);

	//デバイスコンテキストを破棄
	::ReleaseDC(this->hWnd_, this->hDC_);
}

//描画セットアップ
void fw::DrawWGL::setup(CoordI32 mapPos)
{
	//描画領域取得
	Size drawSize;
	this->getDrawSize(drawSize);

	//ビューポート設定
	glViewport(0, 0, drawSize.w, drawSize.h);

	//プロジェクション設定
	std::float64_t left = mapPos.x - drawSize.w / 2;
	std::float64_t right = left + drawSize.w;
	std::float64_t top = mapPos.y - drawSize.h / 2;
	std::float64_t bottom = top + drawSize.h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(left, right, bottom, top, -1, 1);
	glOrtho(left, right, top, bottom, -1, 1);

	//モデルビュー設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//描画カレント
void fw::DrawWGL::makeCurrent(const bool current)
{
	if (current) {
		//カレント設定
		::wglMakeCurrent(this->hDC_, this->hGLRC_);
	}
	else {
		//カレント解除
		::wglMakeCurrent(this->hDC_, nullptr);
	}
}

//描画更新
void fw::DrawWGL::swapBuffers()
{
	::SwapBuffers(this->hDC_);
}

//クリア
void fw::DrawWGL::clear(const Color color)
{
	std::float32_t r = std::float32_t(color.r) / 255.0f;
	std::float32_t g = std::float32_t(color.g) / 255.0f;
	std::float32_t b = std::float32_t(color.b) / 255.0f;
	std::float32_t a = std::float32_t(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//点描画
void fw::DrawWGL::drawPoint(const ACoordI32& coords, const AColor& colors, const std::float32_t size)
{
	//点の大きさを設定
	glPointSize(size);

	//点描画
	glBegin(GL_POINTS);
	{
		//色
		std::int32_t colorNum = colors.getNum();
		for (std::int32_t i = 0; i < colorNum; i++) {
			std::float32_t r = std::float32_t(colors[i].r) / 255.0f;
			std::float32_t g = std::float32_t(colors[i].g) / 255.0f;
			std::float32_t b = std::float32_t(colors[i].b) / 255.0f;
			std::float32_t a = std::float32_t(colors[i].a) / 255.0f;
			glColor4f(r, g, b, a);
		}

		//座標
		std::int32_t coordNum = coords.getNum();
		for (std::int32_t i = 0; i < coordNum; i++) {
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
		}
	}
	glEnd();
}

//ライン描画
void fw::DrawWGL::drawLine(const ACoordI32& coords, const AColor& colors, const std::float32_t width)
{
	//ラインの太さを設定
	glLineWidth(width);

	//ライン描画
	glBegin(GL_LINE_LOOP);
	{
		//色
		std::int32_t colorNum = colors.getNum();
		for (std::int32_t i = 0; i < colorNum; i++) {
			std::float32_t r = std::float32_t(colors[i].r) / 255.0f;
			std::float32_t g = std::float32_t(colors[i].g) / 255.0f;
			std::float32_t b = std::float32_t(colors[i].b) / 255.0f;
			std::float32_t a = std::float32_t(colors[i].a) / 255.0f;
			glColor4f(r, g, b, a);
		}

		//座標
		std::int32_t coordNum = coords.getNum();
		for (std::int32_t i = 0; i < coordNum; i++) {
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
		}
	}
	glEnd();
}

//テクスチャ作成
void fw::DrawWGL::createTexture(const AImage& tex, const Size texSize, std::uint32_t& texId)
{
	//テクスチャ作成
	GLuint textureId;
	glGenTextures(1, &textureId);

	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
	glBindTexture(GL_TEXTURE_2D, 0);

	//テクスチャIDを出力
	texId = textureId;
}

//テクスチャ削除
void fw::DrawWGL::deleteTexture(const std::uint32_t texId)
{
	//テクスチャ削除
	glDeleteTextures(1, &texId);
}

//テクスチャ描画
void fw::DrawWGL::drawTextrue(const ACoordI32& coords, const std::uint32_t texId)
{
	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, texId);

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
		std::int32_t coordNum = coords.getNum();
		std::int32_t i = 0;
		while (i != coordNum) {
			glTexCoord2i(coords[i].x, coords[i].y);
			i++;
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
			i++;
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//テクスチャ割り当て解除
	glBindTexture(GL_TEXTURE_2D, 0);
}

//描画領域取得
void fw::DrawWGL::getDrawSize(Size& drawSize)
{
	RECT rect;
	::GetClientRect(this->hWnd_, &rect);
	drawSize.w = std::int16_t(rect.right - rect.left);
	drawSize.h = std::int16_t(rect.bottom - rect.top);
}
