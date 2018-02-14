#include "DrawWGL.hpp"
#include "Image.hpp"
#include <vector>

//OpenGL
#include <gl/GL.h>
#include <gl/GLU.h>

//FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freetype28.lib")


namespace {
	//フォント文字データ構造体
	struct FontChar {
		std::uint32_t	id_;		//ID(グリフインデックス)
		std::int32_t	w_;			//幅
		std::int32_t	h_;			//高さ
		std::int32_t	distX_;		//調整位置(X方向)
		std::int32_t	distY_;		//調整位置(Y方向,ベースライン基準)
		std::int32_t	nextOfsX_;	//次文字へのオフセット(X方向)
		std::int32_t	baseLine_;	//ベースライン
		std::uint8_t*	buffer_;	//バッファ
	};
}


namespace ui {

	//----------------------------------------------------------
	//
	// WGL描画クラス
	//
	//----------------------------------------------------------

	//コンストラクタ
	DrawWGL::DrawWGL(const HWND hWnd) :
		hWnd_(hWnd), hDC_(nullptr), hGLRC_(nullptr)
	{
	}

	//デストラクタ
	DrawWGL::~DrawWGL()
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
	void DrawWGL::create()
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

		//描画コンテキストをカレントに設定
		::wglMakeCurrent(this->hDC_, this->hGLRC_);

		printf("GL_VENDER: %s\n", glGetString(GL_VENDOR));
		printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
		printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
		//printf("GL_EXTENSIONS: %s\n", glGetString(GL_EXTENSIONS));

		//カレントを解除
		::wglMakeCurrent(this->hDC_, nullptr);
	}

	//描画セットアップ
	void DrawWGL::setup()
	{
		PAINTSTRUCT ps;
		::BeginPaint(this->hWnd_, &ps);
		::EndPaint(this->hWnd_, &ps);

		//ビューポート設定(左下原点)
		RECT rect = { 0 };
		(void)::GetClientRect(this->hWnd_, &rect);
		std::int32_t width = std::int32_t(rect.right - rect.left);
		std::int32_t height = std::int32_t(rect.bottom - rect.top);
		glViewport(0, 0, width, height);

		//プロジェクション設定
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, width, 0.0, height , -10.0, 10.0);

		//モデルビュー設定(左上が原点となるように)
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, -height, 1.0, 0.0, -height, 0.0, 0.0, 1.0, 0.0);
	}

	//描画カレント
	void DrawWGL::makeCurrent(const bool current)
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
	void DrawWGL::swapBuffers()
	{
		::SwapBuffers(this->hDC_);
	}

	//クリア
	void DrawWGL::clear(const ui::ColorUB& color)
	{
		GLclampf r = GLclampf(color.r) / 255.0F;
		GLclampf g = GLclampf(color.g) / 255.0F;
		GLclampf b = GLclampf(color.b) / 255.0F;
		GLclampf a = GLclampf(color.a) / 255.0F;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//イメージ描画
	void DrawWGL::drawImage(const ui::CoordI& coord, const uint8_t* const image, const int32_t width, const int32_t height)
	{
		//テクスチャ作成
		GLuint texId;
		glGenTextures(1, &texId);

		//テクスチャ割り当て
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);
		{
			//色
			//念のためフラグメントカラーを255で初期化する
			glColor4ub(255, 255, 255, 255);

			ui::AreaI area;
			area.xmin = coord.x;
			area.ymin = coord.y - height;
			area.xmax = coord.x + width;
			area.ymax = coord.y;

			//座標
			glTexCoord2f(0.0F, 0.0F);
			glVertex3i(area.xmin, area.ymax, 0);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3i(area.xmax, area.ymax, 0);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3i(area.xmin, area.ymin, 0);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3i(area.xmax, area.ymin, 0);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		//テクスチャ割り当て解除
		glBindTexture(GL_TEXTURE_2D, 0);

		//テクスチャ削除
		glDeleteTextures(1, &texId);
	}

	//文字描画
	void DrawWGL::drawString(const ui::CoordI& coord, const std::wstring& str)
	{
		//文字数
		const std::int32_t numChars = std::int32_t(str.length());

		std::vector<FontChar> ftCharList(numChars);

		//FreeTypeを初期化
		FT_Library ftLibrary;
		FT_Init_FreeType(&ftLibrary);

		//フェイスオブジェクトを作成
		FT_Face ftFace;
		FT_New_Face(ftLibrary, "./ipagp.ttf", 0, &ftFace);

		//ピクセルサイズを指定
		FT_Set_Pixel_Sizes(ftFace, 20, 20);

		//グリフを取得
		FT_GlyphSlot slot = ftFace->glyph;

		//文字数分ラスタライズ
		for (std::int32_t c = 0; c < numChars; c++) {
			FontChar& ftChar = ftCharList[c];

			//グリフインデックスを取得
			FT_UInt glyphIndex = FT_Get_Char_Index(ftFace, str[c]);

			//グリフイメージをロード
			FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_RENDER);

			//文字ラスタ情報を取得
			ftChar.id_ = glyphIndex;
			ftChar.w_ = slot->metrics.width >> 6;
			ftChar.h_ = slot->metrics.height >> 6;
			ftChar.distX_ = slot->metrics.horiBearingX >> 6;
			ftChar.distY_ = slot->metrics.horiBearingY >> 6;
			ftChar.nextOfsX_ = slot->metrics.horiAdvance >> 6;
			ftChar.baseLine_ = ftChar.h_ - ftChar.distY_;
			ftChar.buffer_ = nullptr;
			std::int32_t bufferSize = ftChar.w_ * ftChar.h_;
			if (bufferSize > 0) {
				ftChar.buffer_ = new std::uint8_t[ftChar.w_ * ftChar.h_];
				(void)memcpy_s(ftChar.buffer_, bufferSize, slot->bitmap.buffer, slot->bitmap.width * slot->bitmap.rows);
			}
		}

		//文字列の幅、高さ、ベースラインを取得
		std::int32_t maxWidth = 0;		//文字列の幅
		std::int32_t maxHeight = 0;		//文字列の高さ
		std::int32_t baseLine = 0;		//文字列のベースライン
		for (std::int32_t c = 0; c < numChars; c++) {
			FontChar& ftChar = ftCharList[c];

			//幅(次文字へのオフセットで計算)
			maxWidth += ftChar.nextOfsX_;
			//高さ
			if (maxHeight < ftChar.h_) { maxHeight = ftChar.h_; }
			//ベースライン
			if (baseLine < ftChar.baseLine_) { baseLine = ftChar.baseLine_; }
		}
		//文字列のベースライン分高さを調整
		maxHeight += baseLine;

		//文字列バッファ確保
		std::uint8_t* strBuffer = nullptr;
		std::int32_t strBufferSize = maxWidth * maxHeight;
		if (strBufferSize > 0) {
			strBuffer = new std::uint8_t[strBufferSize];
			memset(strBuffer, 0, strBufferSize);
		}

		if (strBuffer != nullptr) {
			//文字列画像作成
			int32_t staWrite = 0;
			for (std::int32_t c = 0; c < numChars; c++) {
				FontChar& ftChar = ftCharList[c];

				//書き込み位置
				std::int32_t iWrite = staWrite + ftChar.distX_;	//開始位置(自身の文字のX方向の調整分だけ書き込み位置を進めておく)
				iWrite += maxWidth * (maxHeight - ftChar.h_);	//文字列の高さと自身の文字の高さの差分だけ書き込み位置を進める
				iWrite -= maxWidth * baseLine;					//文字列のベースライン分だけ書き込み位置を戻す
				iWrite += maxWidth * ftChar.baseLine_;			//自身の文字のベースライン分だけ書き込み位置を進める

				for (std::int32_t ih = 0; ih < ftChar.h_; ih++) {
					(void)memcpy_s(&strBuffer[iWrite], ftChar.w_, &ftChar.buffer_[ih * ftChar.w_], ftChar.w_);
					iWrite += maxWidth;
				}

				//次文字へのオフセット分進める
				staWrite += ftChar.nextOfsX_;
			}

			//文字列描画
			//描画エリア
			ui::AreaI area;
			area.xmin = coord.x;
			area.ymin = coord.y - maxHeight;
			area.xmax = area.xmin + maxWidth;
			area.ymax = area.ymin + maxHeight;

			GLuint texID;
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, maxWidth, maxHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, strBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLE_STRIP);
			{
				glColor4ub(0, 0, 0, 255);
				//座標
				glTexCoord2f(0.0F, 0.0F);
				glVertex3i(area.xmin, area.ymax, 0);
				glTexCoord2f(1.0F, 0.0F);
				glVertex3i(area.xmax, area.ymax, 0);
				glTexCoord2f(0.0F, 1.0F);
				glVertex3i(area.xmin, area.ymin, 0);
				glTexCoord2f(1.0F, 1.0F);
				glVertex3i(area.xmax, area.ymin, 0);
			}
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &texID);

			//解放
			delete[] strBuffer;
			for (std::int32_t c = 0; c < numChars; c++) {
				delete[] ftCharList[c].buffer_;
			}
		}

		//終了処理
		FT_Done_Face(ftFace);
		FT_Done_FreeType(ftLibrary);
	}
}
