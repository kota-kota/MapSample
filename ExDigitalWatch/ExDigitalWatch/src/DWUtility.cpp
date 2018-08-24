#include "DWUtility.hpp"


namespace {
	//DWWindowインスタンス
	dw::DWWindow* g_dwwindow = nullptr;
	//グローバルミューテックス
	std::mutex g_mtx;
}

namespace dw {

	//----------------------------------------------------------------
	// DWWindowクラス
	//----------------------------------------------------------------

	//作成
	void DWWindow::create(void* native)
	{
		//Windowインスタンスが未生成なら生成する
		g_mtx.lock();
		if (g_dwwindow == nullptr) {
			g_dwwindow = new DWWindow(static_cast<HWND>(native));
		}
		g_mtx.unlock();
	}

	//取得
	DWWindow* DWWindow::get()
	{
		return g_dwwindow;
	}

	//破棄
	void DWWindow::destroy()
	{
		g_mtx.lock();
		if (g_dwwindow != nullptr) {
			delete g_dwwindow;
			g_dwwindow = nullptr;
		}
		g_mtx.unlock();
	}

	//描画開始
	void DWWindow::beginDraw()
	{
		if (this->hGLRC_ == nullptr) {
			//描画コンテキストハンドルを作成
			this->hGLRC_ = ::wglCreateContext(this->hDC_);

			//描画コンテキストをカレントに設定
			::wglMakeCurrent(this->hDC_, this->hGLRC_);
		}

		//ビューポート設定
		glViewport(0, 0, this->size_.width_, this->size_.height_);

		//プロジェクション設定(正射影)
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, this->size_.width_, this->size_.height_, 0.0, -1.0, 1.0);

		//モデルビュー設定
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	//描画終了
	void DWWindow::endDraw()
	{
		::SwapBuffers(this->hDC_);
	}

	//画面塗りつぶし
	void DWWindow::clear(const DWColor& color)
	{
		GLclampf r = static_cast<std::float32_t>(color.r_) / 255.0F;
		GLclampf g = static_cast<std::float32_t>(color.g_) / 255.0F;
		GLclampf b = static_cast<std::float32_t>(color.b_) / 255.0F;
		GLclampf a = static_cast<std::float32_t>(color.a_) / 255.0F;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//文字描画
	void DWWindow::drawText(const DWText& text)
	{
		//フォントサイズ(固定)
		const std::int32_t fontSize = 24;
		//フォント(色固定)
		const DWColor fontColor = { 0, 0, 255, 255 };

		//テキスト文字列のフォントグリフ保持用
		const std::int32_t MAX_GLYPHS = 32;
		FontGlyph glyphs[MAX_GLYPHS];
		std::int32_t numGlyphs = 0;

		//前回のグリフインデックス
		FT_UInt preGlyphIndex = 0;

		//文字列のバウンディングボックス
		FT_BBox stringBBox;

		DWCoord baseCoord = { 0, 0 };

		//フォントサイズ設定
		FT_Set_Char_Size(this->ftFace_, fontSize * 64, 0, 96, 0);

		//テキスト文字分ループ
		for (std::int32_t i = 0; i < text.num_; i++) {
			//処理対象文字のグリフ格納用
			FontGlyph* glyph = &glyphs[numGlyphs];

			//グリフインデックスを取得
			glyph->index_ = FT_Get_Char_Index(this->ftFace_, text.text_[i]);

			//グリフをロード
			FT_Load_Glyph(this->ftFace_, glyph->index_, FT_LOAD_DEFAULT);

			//グリフを描画
			FT_Get_Glyph(this->ftFace_->glyph, &glyph->image_);
			FT_Glyph_To_Bitmap(&glyph->image_, FT_RENDER_MODE_NORMAL, nullptr, 1);

			//寸法情報を取得
			FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph->image_;
			glyph->metrics_.width_ = bit->bitmap.width;
			glyph->metrics_.height_ = bit->bitmap.rows;
			glyph->metrics_.offsetX_ = bit->left;
			glyph->metrics_.offsetY_ = bit->top;
			glyph->metrics_.nextX_ = this->ftFace_->glyph->advance.x >> 6;
			glyph->metrics_.nextY_ = this->ftFace_->glyph->advance.y >> 6;

			//処理対象文字のバウンディングボックスを取得
			FT_BBox bbox;
			FT_Glyph_Get_CBox(glyph->image_, ft_glyph_bbox_pixels, &bbox);

			if (i == 0) {
				stringBBox.xMin = 0;
				stringBBox.xMax = glyph->metrics_.nextX_;
				stringBBox.yMin = bbox.yMin;
				stringBBox.yMax = bbox.yMax;
			}
			else {
				stringBBox.xMin = 0;
				stringBBox.xMax += glyph->metrics_.nextX_;
				if (bbox.yMin < stringBBox.yMin) { stringBBox.yMin = bbox.yMin; }
				if (bbox.yMax > stringBBox.yMax) { stringBBox.yMax = bbox.yMax; }
			}

			//次文字へ
			numGlyphs++;
		}

		//文字列の幅高さ
		const std::int32_t stringW = stringBBox.xMax - stringBBox.xMin;
		const std::int32_t stringH = stringBBox.yMax - stringBBox.yMin;
		const std::int32_t stringSize = stringW * stringH;

		//文字列画像の領域を確保
		std::uint8_t* tex = new std::uint8_t[stringSize];
		memset(tex, 0, stringSize);

		//文字列画像を生成
		FT_Vector pen = { 0, stringBBox.yMax };
		for (std::int32_t i = 0; i < numGlyphs; i++) {
			//処理対象文字のグリフを取得
			FontGlyph* glyph = &glyphs[i];
			FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph->image_;

			const std::int32_t xoffset = pen.x + glyph->metrics_.offsetX_;
			const std::int32_t yoffset = pen.y - glyph->metrics_.offsetY_;
			std::int32_t readOffset = 0;
			std::int32_t writeOffset = xoffset + (yoffset * stringW);
			for (std::int32_t h = 0; h < glyph->metrics_.height_; h++) {
				memcpy_s(tex + writeOffset, stringW, bit->bitmap.buffer + readOffset, glyph->metrics_.width_);
				readOffset += glyph->metrics_.width_;
				writeOffset += stringW;
			}

			pen.x += glyph->metrics_.nextX_;

			//グリフイメージ破棄
			FT_Done_Glyph(glyph->image_);
		}

		//GL描画設定
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		//テクスチャ生成
		GLuint texID;
		glGenTextures(1, &texID);

		//テクスチャバインド
		glBindTexture(GL_TEXTURE_2D, texID);

		//テクスチャロード(ロード後、文字列画像の領域を解放)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, stringW, stringH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tex);
		delete[] tex;

		//テクスチャパラメータ設定
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		DWArea drawArea;
		drawArea.xmin_ = baseCoord.x_;
		drawArea.ymin_ = baseCoord.y_;
		drawArea.xmax_ = baseCoord.x_ + stringW;
		drawArea.ymax_ = baseCoord.y_ + stringH;;

		//描画
		glColor4ub(fontColor.r_, fontColor.g_, fontColor.b_, fontColor.a_);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0F, 0.0F);
		glVertex2i(drawArea.xmin_, drawArea.ymin_);
		glTexCoord2f(1.0F, 0.0F);
		glVertex2i(drawArea.xmax_, drawArea.ymin_);
		glTexCoord2f(0.0F, 1.0F);
		glVertex2i(drawArea.xmin_, drawArea.ymax_);
		glTexCoord2f(1.0F, 1.0F);
		glVertex2i(drawArea.xmax_, drawArea.ymax_);
		glEnd();

		//テクスチャアンバインド
		glBindTexture(GL_TEXTURE_2D, 0);
		//テクスチャ破棄
		glDeleteTextures(1, &texID);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	//画像描画
	void DWWindow::drawBitmap()
	{
		glColor4ub(255, 255, 255, 255);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(0.0F, 100.0F, 0.0F);
		glVertex3f(100.0F, 100.0F, 0.0F);
		glVertex3f(0.0F, 200.0F, 0.0F);
		glVertex3f(100.0F, 200.0F, 0.0F);
		glEnd();
	}

	//コンストラクタ
	DWWindow::DWWindow(HWND hWnd) :
		hWnd_(hWnd), hDC_(nullptr), hGLRC_(nullptr), ftLibrary_(nullptr), ftFace_(nullptr), size_()
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

		//FreeType開始
		FT_Init_FreeType(&this->ftLibrary_);
		//MeiryoUIフォントのフェイスを生成
		FT_New_Face(this->ftLibrary_, "C:/Windows/Fonts/meiryo.ttc", 0, &this->ftFace_);

		//ウィンドウサイズを取得
		RECT rect;
		::GetClientRect(hWnd, &rect);
		this->size_.width_ = rect.right - rect.left;
		this->size_.height_ = rect.bottom - rect.top;
	}

	//デストラクタ
	DWWindow::~DWWindow()
	{
		//フェイスを破棄
		FT_Done_Face(this->ftFace_);
		//FreeType終了
		FT_Done_FreeType(this->ftLibrary_);

		//カレントを解除
		::wglMakeCurrent(this->hDC_, nullptr);

		if (this->hGLRC_ != nullptr) {
			//描画コンテキストハンドルを破棄
			::wglDeleteContext(this->hGLRC_);
		}

		if (this->hDC_ != nullptr) {
			//デバイスコンテキストを破棄
			::ReleaseDC(this->hWnd_, this->hDC_);
		}
	}



	//----------------------------------------------------------------
	// DWFuncクラス
	//----------------------------------------------------------------

	//時刻取得
	DWTime DWFunc::getTime()
	{
		time_t t = time(nullptr);
		struct tm tm;
		(void)localtime_s(&tm, &t);

		DWTime dwTime = { 0 };
		dwTime.h_ = tm.tm_hour;
		dwTime.m_ = tm.tm_min;
		dwTime.s_ = tm.tm_sec;
		strftime(dwTime.str_, sizeof(dwTime.str_), "%H:%M:%S", &tm);
		dwTime.strNum_ = strlen(dwTime.str_);

		return dwTime;
	}
}
