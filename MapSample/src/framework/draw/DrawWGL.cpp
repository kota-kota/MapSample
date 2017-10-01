#include "DrawWGL.hpp"
#include "image/Image.hpp"

#include <new>
#include <Windows.h>
#include <gl/GL.h>


namespace fw
{
	//----------------------------------------------------------
	//
	// WGL描画実装クラス
	//
	//----------------------------------------------------------

	class DrawWGL::Impl {
		HWND	hWnd_;
		HDC		hDC_;
		HGLRC	hGLRC_;

	public:
		//コンストラクタ
		Impl();
		//デストラクタ
		~Impl();
		//作成
		void create(const HWND hWnd);
		//セットアップ
		void setup(const std::CoordI mapPos);
		//描画カレント
		void makeCurrent(const bool current);
		//描画更新
		void swapBuffers();
		//クリア
		void clear(const std::Color color);
		//点描画
		void drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size);
		//ライン描画
		void drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width);
		//テクスチャ作成
		void createTextures(const fw::Image* const tex, std::uint32_t* const texId);
		//テクスチャ削除
		void deleteTextures(const std::uint32_t texId);
		//テクスチャ描画
		void drawTextrue(const std::vector<std::CoordTex>& coords, const std::uint32_t texId);

		//画面サイズ取得
		void getScreenSize(std::Size* const screenSize);
	};
};


//----------------------------------------------------------
//
// WGL描画実装クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawWGL::Impl::Impl()
	: hWnd_(nullptr), hDC_(nullptr), hGLRC_(nullptr)
{
}

//デストラクタ
fw::DrawWGL::Impl::~Impl()
{
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
void fw::DrawWGL::Impl::create(const HWND hWnd)
{
	//ウィンドウハンドルを設定
	this->hWnd_ = hWnd;

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

//描画セットアップ
void fw::DrawWGL::Impl::setup(const std::CoordI mapPos)
{
	//画面サイズ取得
	std::Size screenSize;
	this->getScreenSize(&screenSize);

	//ビューポート設定
	glViewport(0, 0, screenSize.width, screenSize.height);

	//プロジェクション設定
	std::double_t left = mapPos.x - screenSize.width / 2;
	std::double_t right = left + screenSize.width;
	std::double_t top = mapPos.y - screenSize.height / 2;
	std::double_t bottom = top + screenSize.height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(left, right, bottom, top, -1, 1);
	glOrtho(left, right, top, bottom, -1, 1);

	//モデルビュー設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//描画カレント
void fw::DrawWGL::Impl::makeCurrent(const bool current)
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
void fw::DrawWGL::Impl::swapBuffers()
{
	::SwapBuffers(this->hDC_);
}

//クリア
void fw::DrawWGL::Impl::clear(const std::Color color)
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
void fw::DrawWGL::Impl::drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size)
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
void fw::DrawWGL::Impl::drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width)
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

//テクスチャ作成
void fw::DrawWGL::Impl::createTextures(const fw::Image* const tex, std::uint32_t* const texId)
{
	//テクスチャ作成
	GLuint textureId;
	glGenTextures(1, &textureId);

	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->Width(), tex->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->Body());
	glBindTexture(GL_TEXTURE_2D, 0);

	//テクスチャIDを出力
	*texId = textureId;
}

//テクスチャ削除
void fw::DrawWGL::Impl::deleteTextures(const std::uint32_t texId)
{
	//テクスチャ削除
	glDeleteTextures(1, &texId);
}

//テクスチャ描画
void fw::DrawWGL::Impl::drawTextrue(const std::vector<std::CoordTex>& coords, const std::uint32_t texId)
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
		size_t coordNum = coords.size();
		for (size_t i = 0; i < coordNum; i++) {
			glTexCoord2d(coords[i].u, coords[i].v);
			glVertex3i(coords[i].x, coords[i].y, coords[i].z);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//テクスチャ割り当て解除
	glBindTexture(GL_TEXTURE_2D, 0);
}

//描画領域取得
void fw::DrawWGL::Impl::getScreenSize(std::Size* const screenSize)
{
	RECT rect;
	::GetClientRect(this->hWnd_, &rect);
	screenSize->width = std::int16_t(rect.right - rect.left);
	screenSize->height = std::int16_t(rect.bottom - rect.top);
}



//----------------------------------------------------------
//
// WGL描画クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawWGL::DrawWGL() :
	impl_(nullptr)
{
}

//デストラクタ
fw::DrawWGL::~DrawWGL()
{
	if (this->impl_ != nullptr) {
		delete this->impl_;
	}
}

//作成
void fw::DrawWGL::create(void* const hWnd)
{
	if (this->impl_ == nullptr) {
		//実装クラス未作成
		if (hWnd != nullptr) {
			//ウィンドウハンドル指定あり
			this->impl_ = new Impl();
			this->impl_->create(reinterpret_cast<HWND>(hWnd));
		}
	}
}

//描画セットアップ
void fw::DrawWGL::setup(const std::CoordI mapPos)
{
	this->impl_->setup(mapPos);
}

//描画カレント
void fw::DrawWGL::makeCurrent(const bool current)
{
	this->impl_->makeCurrent(current);
}

//描画更新
void fw::DrawWGL::swapBuffers()
{
	this->impl_->swapBuffers();
}

//クリア
void fw::DrawWGL::clear(const std::Color color)
{
	this->impl_->clear(color);
}

//点描画
void fw::DrawWGL::drawPoints(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t size)
{
	this->impl_->drawPoints(coords, colors, size);
}

//ライン描画
void fw::DrawWGL::drawLines(const std::vector<std::CoordI>& coords, const std::vector<std::Color>& colors, const std::float_t width)
{
	this->impl_->drawLines(coords, colors, width);
}

//テクスチャ作成
void fw::DrawWGL::createTextures(const fw::Image* const tex, std::uint32_t* const texId)
{
	this->impl_->createTextures(tex, texId);
}

//テクスチャ削除
void fw::DrawWGL::deleteTextures(const std::uint32_t texId)
{
	this->impl_->deleteTextures(texId);
}

//テクスチャ描画
void fw::DrawWGL::drawTextrue(const std::vector<std::CoordTex>& coords, const std::uint32_t texId)
{
	this->impl_->drawTextrue(coords, texId);
}

//画面サイズ取得
void fw::DrawWGL::getScreenSize(std::Size* const screenSize)
{
	this->impl_->getScreenSize(screenSize);
}
