#include "DrawWGL.hpp"
#include <cstdio>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <png.h>
#include <pngstruct.h>


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng16.lib")


namespace draw {

	//RGBA8888画像の1ピクセルあたりのバイト数
	static const std::uint32_t BYTE_PER_PIXEL_RGBA8888 = 4;

	//----------------------------------------------------------
	//
	// PNGデコードクラス
	//
	//----------------------------------------------------------
	class PngDecorder : public ImageDecorder {
		//メンバ変数
		std::uint8_t*	decodeData_;	//デコードデータ
		std::int32_t	decodeSize_;	//デコードデータサイズ
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ
		std::uint8_t*	pngData_;		//PNGデータ
		std::int32_t	pngSize_;		//PNGデータサイズ
		std::int32_t	rowByte_;		//行バイト数
		std::uint8_t	bitDepth_;		//ビット深度
		std::uint8_t	colorType_;		//カラータイプ

	public:
		//コンストラクタ
		PngDecorder(std::uint8_t* const pngData, const std::int32_t pngSize) :
			decodeData_(nullptr), decodeSize_(0), width_(0), height_(0), pngData_(pngData), pngSize_(pngSize)
		{
		}

		//デストラクタ
		~PngDecorder()
		{
		}

		//デコードデータを取得
		std::uint8_t* PngDecorder::getDecodeData(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height)
		{
			if (decodeSize != nullptr) {
				*decodeSize = this->decodeSize_;
			}
			if (width != nullptr) {
				*width = this->width_;
			}
			if (height != nullptr) {
				*height = this->height_;
			}
			return this->decodeData_;
		}

		//RGBA8888画像へデコード
		void decodeRgba8888()
		{
			const int32_t PNG_BYTES_TO_CHECK = 4;

			//PNGシグネチャのチェック
			png_byte sig[PNG_BYTES_TO_CHECK];
			(void)memcpy_s(sig, PNG_BYTES_TO_CHECK, this->pngData_, PNG_BYTES_TO_CHECK);
			if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK) == 0) {
				//PNG画像

				//PNG構造ポインタ作成
				png_structp png_str = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

				//PNG情報ポインタ作成
				png_infop png_info = png_create_info_struct(png_str);

				//シグネチャ読み込み済み
				png_set_sig_bytes(png_str, PNG_BYTES_TO_CHECK);
				this->pngData_ += PNG_BYTES_TO_CHECK;

				//PNG読み込みコールバック関数を登録
				png_set_read_fn(png_str, &this->pngData_, this->callbackReadPng);

				//PNG読み込み
				png_read_info(png_str, png_info);

				//IHDRチャンクの各種情報取得
				this->width_ = png_get_image_width(png_str, png_info);
				this->height_ = png_get_image_height(png_str, png_info);
				this->rowByte_ = int32_t(png_get_rowbytes(png_str, png_info));
				this->bitDepth_ = png_get_bit_depth(png_str, png_info);
				this->colorType_ = png_get_color_type(png_str, png_info);

				//デコードデータのメモリを確保
				if (this->decodeData_ != nullptr) {
					//前回データは解放
					delete[] this->decodeData_;
				}
				this->decodeSize_ = this->width_ + this->height_ * BYTE_PER_PIXEL_RGBA8888;
				this->decodeData_ = new uint8_t[this->decodeSize_];

				//デコード後の画像データを格納するメモリを確保(本関数の最後に解放)
				png_size_t pngSize = this->width_ * sizeof(png_bytep) + this->height_ * this->rowByte_;
				png_byte* tmp = new png_byte[pngSize];
				png_bytepp png = (png_bytepp)tmp;

				//png_read_imageには各行へのポインタを渡すため、2次元配列化
				png_bytep wp = (png_bytep)&png[this->height_];
				for (std::int32_t h = 0; h < this->height_; h++) {
					png[h] = wp;
					wp += this->rowByte_;
				}

				//PNGイメージ読み込み
				png_read_image(png_str, png);

				switch (this->colorType_) {
				case PNG_COLOR_TYPE_GRAY:		//0:グレー
					this->decodeRgba8888FromGrayScalePng(&this->decodeData_, png);
					break;
				case PNG_COLOR_TYPE_RGB:		//2:トゥルーカラー
					this->decodeRgba8888FromTrueColorPng(&this->decodeData_, png);
					break;
				case PNG_COLOR_TYPE_PALETTE:	//3:パレット
					this->decodeRgba8888FromPalletePng(&this->decodeData_, png, png_str, png_info);
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:	//4:グレー+アルファ
					break;
				case PNG_COLOR_TYPE_RGB_ALPHA:	//6:トゥルーカラー+アルファ
					break;
				default:
					break;
				};

				//デコード後の画像データを格納するメモリを解放
				delete[] png;

				png_destroy_info_struct(png_str, &png_info);
				png_destroy_read_struct(&png_str, nullptr, nullptr);
			}
			else {
				//PNG画像でない
			}
		}

	private:
		//PNG読み込みコールバック
		static void callbackReadPng(png_structp png_str, png_bytep data, png_size_t length)
		{
			png_bytepp buf = (png_bytepp)png_get_io_ptr(png_str);
			memcpy_s(data, length, *buf, length);
			*buf += length;
		}

		//グレーPNG画像からRGBA8888画像へデコード
		void decodeRgba8888FromGrayScalePng(std::uint8_t** const outData, const png_bytepp png)
		{
			if (this->bitDepth_ <= 8) {
				//ビット深度が1bit,2bit,4bit,8bitの場合

				//ビット深度で表現できる最大値
				png_byte bitMaxValue = (0x01 << this->bitDepth_) - 1;

				//グレーサンプル値(輝度に応じたグレーカラー取得に必要)
				png_byte graySample = 255 / bitMaxValue;

				//出力データへデコード後の画像データを設定
				for (std::int32_t h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
					std::int32_t readOffset = 0;

					//ビット深度に応じたビットオフセット
					png_int_16 bitOfs = 8 - this->bitDepth_;

					for (std::int32_t w = 0; w < this->width_; w++) {
						//輝度を取得
						png_byte brightness = (wp[readOffset] >> bitOfs) & bitMaxValue;

						//輝度に応じたグレーカラーを取得
						png_byte grayColor = graySample * brightness;

						//出力データへRGBA値を設定
						*((*outData) + writeOffset + 0) = grayColor;
						*((*outData) + writeOffset + 1) = grayColor;
						*((*outData) + writeOffset + 2) = grayColor;
						*((*outData) + writeOffset + 3) = 255;

						//ビットオフセットを更新
						bitOfs -= this->bitDepth_;
						if (bitOfs < 0) {
							//次の読み込み位置に更新
							bitOfs = 8 - this->bitDepth_;
							readOffset++;
						}
						//書き込み位置を更新
						writeOffset += BYTE_PER_PIXEL_RGBA8888;
					}
				}
			}
			else {
				//ビット深度が1bit,2bit,4bit,8bit以外の場合
				//ビット深度が16bitの可能性があるが、現状48bitカラーは表現できないので実装しない
				//16bit以外でここに来た場合は異常
			}
		}

		//トゥルーカラーPNG画像からRGBA8888画像へデコード
		void decodeRgba8888FromTrueColorPng(std::uint8_t** const outData, const png_bytepp png)
		{
			if (this->bitDepth_ == 8) {
				//ビット深度が8bitの場合

				//出力データへデコード後の画像データを設定
				for (std::int32_t h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
					std::int32_t readOffset = 0;

					for (std::int32_t w = 0; w < this->width_; w++) {
						//出力データへRGBA値を設定
						*((*outData) + writeOffset + 0) = wp[readOffset + 0];
						*((*outData) + writeOffset + 1) = wp[readOffset + 1];
						*((*outData) + writeOffset + 2) = wp[readOffset + 2];
						*((*outData) + writeOffset + 3) = 255;

						//書き込み位置を更新
						writeOffset += BYTE_PER_PIXEL_RGBA8888;
						readOffset += 3;
					}
				}
			}
			else {
				//ビット深度が8bit以外は何もしない
				//ビット深度が16bitの可能性があるが、現状48bitカラーは表現できないので実装しない
				//16bit以外でここに来た場合は異常
			}
		}

		//パレットPNG画像からRGBA8888画像へデコード
		void decodeRgba8888FromPalletePng(std::uint8_t** const outData, const png_bytepp png, png_structp png_str, png_infop png_info)
		{
			//PLTEチャンク読み込み
			png_colorp pallete = nullptr;
			std::int32_t palleteNum = 0;
			png_get_PLTE(png_str, png_info, &pallete, &palleteNum);

			if ((pallete != nullptr) && (palleteNum > 0)) {
				//パレットデータ取得成功

				//出力データへデコード後の画像データを設定
				for (std::int32_t h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
					std::int32_t readOffset = 0;

					//ビット深度に応じたビットオフセットとビットマスク
					png_int_16 bitOfs = 8 - this->bitDepth_;
					png_byte bitMask = (0x01 << this->bitDepth_) - 1;

					for (std::int32_t w = 0; w < this->width_; w++) {
						//画像データはパレットインデックス
						png_byte palleteIndex = (wp[readOffset] >> bitOfs) & bitMask;

						//出力データへRGBA値を設定
						*((*outData) + writeOffset + 0) = pallete[palleteIndex].red;
						*((*outData) + writeOffset + 1) = pallete[palleteIndex].green;
						*((*outData) + writeOffset + 2) = pallete[palleteIndex].blue;
						*((*outData) + writeOffset + 3) = 255;

						//ビットオフセットを更新
						bitOfs -= this->bitDepth_;
						if (bitOfs < 0) {
							//次の読み込み位置に更新
							bitOfs = 8 - this->bitDepth_;
							readOffset++;
						}
						//書き込み位置を更新
						writeOffset += BYTE_PER_PIXEL_RGBA8888;
					}
				}
			}
			else {
				//パレットデータ取得失敗
			}
		}
	};


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

		//ビューポート設定
		RECT rect = { 0 };
		(void)::GetClientRect(this->hWnd_, &rect);
		std::int32_t width = std::int32_t(rect.right - rect.left);
		std::int32_t height = std::int32_t(rect.bottom - rect.top);
		glViewport(0, 0, width, height);

		//プロジェクション設定
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho();
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
	void DrawWGL::clear(const draw::ColorUB& color)
	{
		GLclampf r = GLclampf(color.r) / 255.0F;
		GLclampf g = GLclampf(color.g) / 255.0F;
		GLclampf b = GLclampf(color.b) / 255.0F;
		GLclampf a = GLclampf(color.a) / 255.0F;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	/*
	//イメージ描画
	void DrawWGL::drawImage(const std::CoordI& coord, const Image& image)
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
			//念のためフラグメントカラーを255で初期化する
			glColor4ub(255, 255, 255, 255);

			std::AreaI area;
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

		//テクスチャ割り当て解除
		glBindTexture(GL_TEXTURE_2D, 0);

		//テクスチャ削除
		glDeleteTextures(1, &texId);
	}

	//文字描画
	void DrawWGL::drawString(const std::CoordI& coord, const wchar_t* const str)
	{
		std::int32_t textCnt = std::int32_t(wcslen(str));

		std::vector<Character> charList;
		charList.resize(textCnt);

		std::CoordI dispCoord = coord;
		for (std::int32_t i = 0; i < textCnt; i++) {
			//ラスタライズ
			this->font_->rasterize(str[i], &charList[i]);

			std::AreaI area;
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

			glColor4ub(0, 0, 0, 255);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3i(area.xmin, area.ymin, 0);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3i(area.xmax, area.ymin, 0);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3i(area.xmin, area.ymax, 0);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3i(area.xmax, area.ymax, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &texID);
		}
	}
	*/
}
