#include "DrawWGL.hpp"
#include "image/Image.hpp"
#include "image/Font.hpp"

#include <new>
#include <gl/GL.h>


//----------------------------------------------------------
//
// WGL描画クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawWGL::DrawWGL(const HWND hWnd)
	: hWnd_(hWnd), hDC_(nullptr), hGLRC_(nullptr), font_(nullptr)
{
}

//デストラクタ
fw::DrawWGL::~DrawWGL()
{
	if (this->font_ != nullptr) {
		//フォントオブジェクト解放
		delete this->font_;
	}

	if (this->hGLRC_ != nullptr) {
		//描画コンテキストハンドルを破棄
		::wglDeleteContext(this->hGLRC_);
	}

	if (this->hDC_ != nullptr) {
		//デバイスコンテキストを破棄
		::ReleaseDC(this->hWnd_, this->hDC_);
	}
}

//作成
void fw::DrawWGL::create()
{
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

	//フォントオブジェクトを作成
	this->font_ = new image::Font();
}

//描画セットアップ
void fw::DrawWGL::setup(const std::CoordI mapPos)
{
	PAINTSTRUCT ps;
	::BeginPaint(this->hWnd_, &ps);
	::EndPaint(this->hWnd_, &ps);

	//画面幅高さを取得
	std::WH screenwh = this->getScreenWH();

	//ビューポート設定
	glViewport(0, 0, screenwh.width, screenwh.height);

	//プロジェクション設定
	std::double_t left = mapPos.x - screenwh.width / 2;
	std::double_t right = left + screenwh.width;
	std::double_t top = mapPos.y + screenwh.height / 2;
	std::double_t bottom = top - screenwh.height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -1, 1);

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
void fw::DrawWGL::clear(const std::Color color)
{
	std::float_t r = static_cast<std::float_t>(color.r) / 255.0f;
	std::float_t g = static_cast<std::float_t>(color.g) / 255.0f;
	std::float_t b = static_cast<std::float_t>(color.b) / 255.0f;
	std::float_t a = static_cast<std::float_t>(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//点描画
void fw::DrawWGL::drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size)
{
	//点の大きさを設定
	glPointSize(size);

	//点描画
	glBegin(GL_POINTS);
	{
		//色
		size_t colorNum = colors.size();
		for (size_t i = 0; i < colorNum; i++) {
			std::float_t r = static_cast<std::float_t>(colors[i].r) / 255.0f;
			std::float_t g = static_cast<std::float_t>(colors[i].g) / 255.0f;
			std::float_t b = static_cast<std::float_t>(colors[i].b) / 255.0f;
			std::float_t a = static_cast<std::float_t>(colors[i].a) / 255.0f;
			glColor4f(r, g, b, a);
		}

		//座標
		size_t coordNum = coords.size();
		for (size_t i = 0; i < coordNum; i++) {
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
		}
	}
	glEnd();
}

//ライン描画
void fw::DrawWGL::drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width)
{
	//ラインの太さを設定
	glLineWidth(width);

	//ライン描画
	glBegin(GL_LINE_LOOP);
	{
		//色
		size_t colorNum = colors.size();
		for (size_t i = 0; i < colorNum; i++) {
			std::float_t r = static_cast<std::float_t>(colors[i].r) / 255.0f;
			std::float_t g = static_cast<std::float_t>(colors[i].g) / 255.0f;
			std::float_t b = static_cast<std::float_t>(colors[i].b) / 255.0f;
			std::float_t a = static_cast<std::float_t>(colors[i].a) / 255.0f;
			glColor4f(r, g, b, a);
		}

		//座標
		size_t coordNum = coords.size();
		for (size_t i = 0; i < coordNum; i++) {
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
		}
	}
	glEnd();
}

//イメージ描画
void fw::DrawWGL::drawImage(const std::CoordI coord, const fw::Image& image)
{
	//イメージをデコード
	ImageDecorder decorder;
	(void)decorder.decode(image);

	//デコード画像を取得
	std::int32_t decodeSize = 0;
	std::int32_t width = 0;
	std::int32_t height = 0;
	std::uint8_t* decode = decorder.getDecodeData(&decodeSize, &width, &height);

	//テクスチャ作成
	GLuint texId;
	glGenTextures(1, &texId);

	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decode);
	glBindTexture(GL_TEXTURE_2D, 0);

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

		std::Area area;
		area.xmin = coord.x;
		area.ymin = coord.y - height;
		area.xmax = coord.x + width;
		area.ymax = coord.y;

		//座標
		glTexCoord2d(0.0, 0.0);
		glVertex3i(area.xmin, area.ymax, 0);
		glTexCoord2d(1.0, 0.0);
		glVertex3i(area.xmax, area.ymax, 0);
		glTexCoord2d(0.0, 1.0);
		glVertex3i(area.xmin, area.ymin, 0);
		glTexCoord2d(1.0, 1.0);
		glVertex3i(area.xmax, area.ymin, 0);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//テクスチャ割り当て解除
	glBindTexture(GL_TEXTURE_2D, 0);

	//テクスチャ削除
	glDeleteTextures(1, &texId);
}

//文字描画
void fw::DrawWGL::drawString(const std::CoordI coord, wchar_t* const str)
{
	std::int32_t textCnt = std::int32_t(wcslen(str));

	std::vector<image::Character> charList;
	charList.resize(textCnt);

	std::CoordI dispCoord = coord;
	for (std::int32_t i = 0; i < textCnt; i++) {
		//ラスタライズ
		this->font_->rasterize(str[i], &charList[i]);

		std::Area area;
		area.xmin = dispCoord.x + charList[i].bl;
		area.xmax = dispCoord.x + charList[i].bw;
		area.ymin = dispCoord.y - (charList[i].bh - charList[i].bt);
		area.ymax = dispCoord.y + charList[i].bt;

		//次の文字位置
		dispCoord.x += charList[i].ax >> 6;
		dispCoord.y += charList[i].ay >> 6;

		GLuint texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, charList[i].bw, charList[i].bh, 0, GL_ALPHA, GL_UNSIGNED_BYTE, charList[i].buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);

		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(0.0, 1.0);
		glVertex2i(area.xmin, area.ymin);
		glTexCoord2d(1.0, 1.0);
		glVertex2i(area.xmax, area.ymin);
		glTexCoord2d(0.0, 0.0);
		glVertex2i(area.xmin, area.ymax);
		glTexCoord2d(1.0, 0.0);
		glVertex2i(area.xmax, area.ymax);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &texID);
	}
}

//画面幅高さを取得
std::WH fw::DrawWGL::getScreenWH()
{
	RECT rect = { 0 };
	(void)::GetClientRect(this->hWnd_, &rect);

	std::WH wh = { 0 };
	wh.width = std::int32_t(rect.right - rect.left);
	wh.height = std::int32_t(rect.bottom - rect.top);

	return wh;
}
