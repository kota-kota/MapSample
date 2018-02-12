#include "Image.hpp"

#include <cstdio>
#include <png.h>
#include <pngstruct.h>

#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "zlib.lib")

namespace {

	//----------------------------------------------------------
	//
	// 共通定義
	//
	//----------------------------------------------------------

	//RGBA8888画像の1ピクセルあたりのバイト数
	static const std::uint32_t BYTE_PER_PIXEL_RGBA8888 = 4;

	//カラー構造体
	struct Color {
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
	};


	//----------------------------------------------------------
	//
	// バイナリ操作メソッド群
	//
	//----------------------------------------------------------

	class ByteReader {
	public:
		//1バイトを読み込み(LE)
		static void read1ByteLe(const std::uint8_t* const data, uint8_t* const readData)
		{
			*readData = *(data + 0);
		}

		//2バイトを読み込み(LE)
		static void read2ByteLe(const std::uint8_t* const data, uint16_t* const readData)
		{
			*readData = 0;
			uint16_t v1 = uint16_t(*(data + 0));
			*readData |= v1 << 0;
			uint16_t v2 = uint16_t(*(data + 1));
			*readData |= v2 << 8;
		}

		//4バイトを読み込み(LE)
		static void read4ByteLe(const std::uint8_t* const data, uint32_t* const readData)
		{
			*readData = 0;
			uint32_t v1 = uint32_t(*(data + 0));
			*readData |= v1 << 0;
			uint32_t v2 = uint32_t(*(data + 1));
			*readData |= v2 << 8;
			uint32_t v3 = uint32_t(*(data + 2));
			*readData |= v3 << 16;
			uint32_t v4 = uint32_t(*(data + 3));
			*readData |= v4 << 24;
		}
	};


	//----------------------------------------------------------
	//
	// 画像処理I/Fクラス
	//
	//----------------------------------------------------------
	class ImageIF {
	public:
		//デストラクタ
		virtual ~ImageIF() {}
		//RGBA8888画像へデコード(出力したデコードデータはユーザが解放する必要がある)
		virtual std::uint8_t* decodeRgba8888(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height) = 0;
	};


	//----------------------------------------------------------
	//
	// PNG画像処理クラス
	//
	//----------------------------------------------------------
	class Png : public ImageIF {
		//メンバ変数
		std::uint8_t*	pngData_;		//PNGデータ
		std::int32_t	pngSize_;		//PNGデータサイズ
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ
		std::int32_t	rowByte_;		//行バイト数
		std::uint8_t	bitDepth_;		//ビット深度
		std::uint8_t	colorType_;		//カラータイプ

	public:
		//コンストラクタ
		Png(std::uint8_t* const pngData, const std::int32_t pngSize) :
			pngData_(pngData), pngSize_(pngSize), width_(0), height_(0), rowByte_(0), bitDepth_(0), colorType_(0)
		{
		}

		//デストラクタ
		virtual ~Png()
		{
		}

		//RGBA8888画像へデコード(出力したデコードデータはユーザが解放する必要がある)
		virtual std::uint8_t* decodeRgba8888(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height)
		{
			//デコードデータ用
			int32_t decSize = 0;
			std::uint8_t* decData = nullptr;

			static const std::int32_t PNG_BYTES_TO_CHECK = 4;
			std::uint8_t* pngData = this->pngData_;

			//PNGシグネチャのチェック
			png_byte sig[PNG_BYTES_TO_CHECK];
			(void)memcpy_s(sig, PNG_BYTES_TO_CHECK, pngData, PNG_BYTES_TO_CHECK);
			if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK) == 0) {
				//PNG画像

				//PNG構造ポインタ作成
				png_structp png_str = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

				//PNG情報ポインタ作成
				png_infop png_info = png_create_info_struct(png_str);

				//シグネチャ読み込み済み
				png_set_sig_bytes(png_str, PNG_BYTES_TO_CHECK);
				pngData += PNG_BYTES_TO_CHECK;

				//PNG読み込みコールバック関数を登録
				png_set_read_fn(png_str, &pngData, this->callbackReadPng);

				//PNG読み込み
				png_read_info(png_str, png_info);

				//IHDRチャンクの各種情報取得
				this->width_ = png_get_image_width(png_str, png_info);
				this->height_ = png_get_image_height(png_str, png_info);
				this->rowByte_ = int32_t(png_get_rowbytes(png_str, png_info));
				this->bitDepth_ = png_get_bit_depth(png_str, png_info);
				this->colorType_ = png_get_color_type(png_str, png_info);

				//デコード後の画像データを格納するメモリを確保(本関数の最後に解放)
				png_size_t pngSize = this->height_ * sizeof(png_bytep) + this->height_ * this->rowByte_;
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

				//デコードデータの格納用メモリを確保
				decSize = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
				decData = new std::uint8_t[decSize];

				switch (this->colorType_) {
				case PNG_COLOR_TYPE_GRAY:		//0:グレー
					this->decodeRgba8888FromGrayScalePng(&decData, png);
					break;
				case PNG_COLOR_TYPE_RGB:		//2:トゥルーカラー
					this->decodeRgba8888FromTrueColorPng(&decData, png, false);
					break;
				case PNG_COLOR_TYPE_PALETTE:	//3:パレット
					this->decodeRgba8888FromPalletePng(&decData, png, png_str, png_info);
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:	//4:グレー+アルファ
					break;
				case PNG_COLOR_TYPE_RGB_ALPHA:	//6:トゥルーカラー+アルファ
					this->decodeRgba8888FromTrueColorPng(&decData, png, true);
					break;
				default:
					break;
				};

				//デコード後の画像データを格納するメモリを解放
				delete[] png;

				//PNG情報ポインタとPNG構造ポインタを解放
				png_destroy_info_struct(png_str, &png_info);
				png_destroy_read_struct(&png_str, nullptr, nullptr);
			}
			else {
				//PNG画像でない
			}

			//出力
			if (decodeSize != nullptr) { *decodeSize = decSize; }
			if (width != nullptr) { *width = this->width_; }
			if (height != nullptr) { *height = this->height_; }
			return decData;
		}

	private:
		//PNG読み込みコールバック
		static void callbackReadPng(png_structp pngStr, png_bytep data, png_size_t length)
		{
			png_bytepp buf = (png_bytepp)png_get_io_ptr(pngStr);
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
		void decodeRgba8888FromTrueColorPng(std::uint8_t** const outData, const png_bytepp png, const bool isAlpha)
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
						if (isAlpha) {
							*((*outData) + writeOffset + 3) = wp[readOffset + 3];
							readOffset += 4;
						}
						else {
							*((*outData) + writeOffset + 3) = 255;
							readOffset += 3;
						}

						//書き込み位置を更新
						writeOffset += BYTE_PER_PIXEL_RGBA8888;
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
	};	//Png



	//----------------------------------------------------------
	//
	// Bitmap画像処理クラス
	//
	//----------------------------------------------------------
	class Bitmap : public ImageIF {

		static const std::uint32_t PALLETE_MAXNUM = 256;	//パレット最大数(256色)

		//Bitmapファイルヘッダ(Windows,OS/2共通)
		static const std::int16_t BFH_HEADERSIZE = 14;
		static const std::int16_t BFH_FILETYPE_OFS = 0;		//ファイルタイプ
		static const std::int16_t BFH_FILESIZE_OFS = 2;		//ファイルサイズ[byte]
		static const std::int16_t BFH_RESERVED1_OFS = 6;	//予約領域1
		static const std::int16_t BFH_RESERVED2_OFS = 8;	//予約領域2
		static const std::int16_t BFH_IMAGEOFS_OFS = 10;	//ファイル先頭から画像データまでのオフセット[byte]

		//Bitmap情報ヘッダ(Windows)
		static const std::int16_t BIH_HEADERSIZE = 40;
		static const std::int16_t BIH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
		static const std::int16_t BIH_WIDTH_OFS = 18;		//画像の幅[pixel]
		static const std::int16_t BIH_HEIGHT_OFS = 22;		//画像の高さ[pixel]
		static const std::int16_t BIH_PLANENUM_OFS = 26;	//プレーン数
		static const std::int16_t BIH_BITCOUNT_OFS = 28;	//色ビット数[bit]
		static const std::int16_t BIH_COMPRESSION_OFS = 30;	//圧縮形式
		static const std::int16_t BIH_IMGDATASIZE_OFS = 34;	//画像データサイズ[byte]
		static const std::int16_t BIH_XDPM_OFS = 38;		//水平解像度[dot/m]
		static const std::int16_t BIH_YDPM_OFS = 42;		//垂直解像度[dot/m]
		static const std::int16_t BIH_PALLETENUM_OFS = 46;	//パレット数[使用色数]
		static const std::int16_t BIH_IMPCOLORNUM_OFS = 50;	//重要色数
		static const std::int16_t BIH_PALLETE_OFS = 54;		//パレット

		//Bitmapコアヘッダ(OS/2)
		static const std::int16_t BCH_HEADERSIZE = 12;
		static const std::int16_t BCH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
		static const std::int16_t BCH_WIDTH_OFS = 18;		//画像の幅[pixel]
		static const std::int16_t BCH_HEIGHT_OFS = 20;		//画像の高さ[pixel]
		static const std::int16_t BCH_PLANENUM_OFS = 22;	//プレーン数
		static const std::int16_t BCH_BITCOUNT_OFS = 24;	//色ビット数[bit]
		static const std::int16_t BCH_PALLETE_OFS = 26;		//パレット

		//Bitmap圧縮形式
		static const std::uint32_t COMPRESSION_BI_RGB = 0;			//無圧縮
		static const std::uint32_t COMPRESSION_BI_RLE8 = 1;			//ランレングス圧縮[8bpp]
		static const std::uint32_t COMPRESSION_BI_RLE4 = 2;			//ランレングス圧縮[4bpp]
		static const std::uint32_t COMPRESSION_BI_BITFIELDS = 3;	//ビットフィールド

		//Bitmapフォーマットタイプ
		enum EN_BmpFormat {
			D_BMPFORMAT_INVALID,		//無効
			D_BMPFORMAT_WINDOWS,		//Windowsフォーマット
			D_BMPFORMAT_OS2,			//OS/2フォーマット
		};

		//メンバ変数
		std::uint8_t*	bmpData_;		//Bitmapデータ
		std::int32_t	bmpSize_;		//Bitmapデータサイズ
		EN_BmpFormat	format_;		//Bitmapフォーマットタイプ
		std::int32_t	fileSize_;		//ファイルサイズ
		std::int32_t	imageOffset_;	//画像データまでのオフセット
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ
		std::int32_t	bitCount_;		//ビット数
		std::int32_t	compression_;	//圧縮形式
		std::int32_t	imageSize_;		//画像データサイズ
		std::int32_t	palleteNum_;	//パレット数
		std::int32_t	palleteByte_;	//1パレットあたりのバイト数
		std::int32_t	palleteOffset_;	//パレットまでのオフセット

	public:
		//コンストラクタ
		Bitmap(std::uint8_t* const bmpData, const std::int32_t bmpSize) :
			bmpData_(bmpData), bmpSize_(bmpSize), format_(EN_BmpFormat::D_BMPFORMAT_INVALID), fileSize_(0), imageOffset_(0),
			width_(0), height_(0), bitCount_(0), compression_(0), imageSize_(0), palleteNum_(0), palleteByte_(0), palleteOffset_(0)
		{
		}

		//デストラクタ
		virtual ~Bitmap()
		{
		}

		//RGBA8888画像へデコード(出力したデコードデータはユーザが解放する必要がある)
		virtual std::uint8_t* decodeRgba8888(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height)
		{
			//デコードデータ用
			int32_t decSize = 0;
			std::uint8_t* decData = nullptr;

			//ファイルタイプを取得
			std::uint8_t fileType[2];
			ByteReader::read1ByteLe(this->bmpData_ + BFH_FILETYPE_OFS, &fileType[0]);
			ByteReader::read1ByteLe(this->bmpData_ + BFH_FILETYPE_OFS + 1, &fileType[1]);
			if ((fileType[0] == 'B') && (fileType[1] == 'M')) {
				//Bitmap画像

				//ファイルサイズを取得
				std::uint32_t fileSize = 0;
				ByteReader::read4ByteLe(this->bmpData_ + BFH_FILESIZE_OFS, &fileSize);
				this->fileSize_ = fileSize;

				//ファイル先頭から画像データまでのオフセットを取得
				std::uint32_t imageOffset = 0;
				ByteReader::read4ByteLe(this->bmpData_ + BFH_IMAGEOFS_OFS, &imageOffset);
				this->imageOffset_ = imageOffset;

				//情報ヘッダサイズを取得
				std::uint32_t infoHeaderSize = 0;
				ByteReader::read4ByteLe(this->bmpData_ + BIH_HEADERSIZE_OFS, &infoHeaderSize);

				//情報ヘッダサイズに応じてフォーマットタイプを選択
				if (infoHeaderSize == BIH_HEADERSIZE) {
					//Windowsフォーマット
					this->format_ = D_BMPFORMAT_WINDOWS;
					//Bitmap情報ヘッダ(Windows)読み込み
					this->readInfoHeaderWindows();
				}
				else if (infoHeaderSize == BCH_HEADERSIZE) {
					//OS/2フォーマット
					this->format_ = D_BMPFORMAT_OS2;
					//Bitmap情報ヘッダ(OS/2)読み込み
					this->readInfoHeaderOS2();
				}
				else {
					//異常
					this->format_ = D_BMPFORMAT_INVALID;
				}

				//デコードデータの格納用メモリを確保
				decSize = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
				decData = new std::uint8_t[decSize];

				switch (this->bitCount_) {
				case 1:		//1bit
				case 4:		//4bit
				case 8:		//8bit
					this->decodeRgba8888FromPalleteBitmap(&decData);
					break;
				case 24:	//24bit
				case 32:	//32bit
					this->decodeRgba8888FromTrueColorBitmap(&decData);
					break;
				default:
					break;
				}
			}
			else {
				//Bitmap画像でない
			}

			//出力
			if (decodeSize != nullptr) { *decodeSize = decSize; }
			if (width != nullptr) { *width = this->width_; }
			if (height != nullptr) { *height = this->height_; }
			return decData;
		}

	private:
		//Bitmap情報ヘッダ(Windows)読み込み
		void readInfoHeaderWindows()
		{
			//画像の幅と高さを取得
			std::uint32_t width = 0;
			std::uint32_t height = 0;
			ByteReader::read4ByteLe(this->bmpData_ + BIH_WIDTH_OFS, &width);
			ByteReader::read4ByteLe(this->bmpData_ + BIH_HEIGHT_OFS, &height);

			//色ビット数を取得
			std::uint16_t bitCount = 0;
			ByteReader::read2ByteLe(this->bmpData_ + BIH_BITCOUNT_OFS, &bitCount);

			//圧縮形式を取得
			std::uint32_t compression = 0;
			ByteReader::read4ByteLe(this->bmpData_ + BIH_COMPRESSION_OFS, &compression);

			//画像データサイズを取得
			std::uint32_t imageSize = 0;
			ByteReader::read4ByteLe(this->bmpData_ + BIH_IMGDATASIZE_OFS, &imageSize);

			//パレット数を取得
			std::uint32_t palleteNum = 0;
			ByteReader::read4ByteLe(this->bmpData_ + BIH_PALLETENUM_OFS, &palleteNum);
			if ((palleteNum == 0) && (bitCount <= 8)) {
				//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
				palleteNum = (1 << bitCount);
			}

			//メンバ変数に設定
			this->width_ = int32_t(width);
			this->height_ = int32_t(height);
			this->bitCount_ = int32_t(bitCount);
			this->compression_ = int32_t(compression);
			this->imageSize_ = int32_t(imageSize);
			this->palleteNum_ = int32_t(palleteNum);
			this->palleteByte_ = int32_t(4);
			this->palleteOffset_ = uint32_t(BIH_PALLETE_OFS);
		}

		//Bitmap情報ヘッダ(OS/2)読み込み
		void readInfoHeaderOS2()
		{
			//画像の幅と高さを取得
			std::uint16_t width = 0;
			std::uint16_t height = 0;
			ByteReader::read2ByteLe(this->bmpData_ + BCH_WIDTH_OFS, &width);
			ByteReader::read2ByteLe(this->bmpData_ + BCH_HEIGHT_OFS, &height);

			//色ビット数を取得
			std::uint16_t bitCount = 0;
			ByteReader::read2ByteLe(this->bmpData_ + BCH_BITCOUNT_OFS, &bitCount);

			//パレット数を取得
			std::uint32_t palleteNum = 0;
			if (bitCount <= 8) {
				//ビット数が8以下の場合は、パレット数を計算
				palleteNum = (1 << bitCount);
			}

			//メンバ変数に設定
			this->width_ = int32_t(width);
			this->height_ = int32_t(height);
			this->bitCount_ = int32_t(bitCount);
			this->compression_ = int32_t(0);
			this->imageSize_ = int32_t(0);
			this->palleteNum_ = int32_t(palleteNum);
			this->palleteByte_ = int32_t(3);
			this->palleteOffset_ = int32_t(BCH_PALLETE_OFS);
		}

		//パディングバイト数を取得
		//画像データサイズ = ((((色ビット数 * 画像の幅) + パティング) × 画像の高さ) / 8)から計算
		std::uint32_t getPaddingByte()
		{
			//画像データサイズを取得
			std::uint32_t imageSize = this->imageSize_;
			if (imageSize == 0) {
				imageSize = this->fileSize_ - this->imageOffset_;
			}

			//パディングビット数を計算
			std::uint32_t paddingBit = ((imageSize * 8) / this->height_) - (this->width_ * this->bitCount_);

			return paddingBit / 8;
		}

		//パレットデータを取得
		void getPalleteData(Color* const pallete)
		{
			//パレットデータを取得
			std::uint32_t readOffset = this->palleteOffset_;
			std::uint32_t writeOffset = 0;
			for (std::uint16_t p = 0; p < this->palleteNum_; p++) {
				//青→緑→赤
				ByteReader::read1ByteLe(this->bmpData_ + readOffset + 0, &(pallete[writeOffset].b));
				ByteReader::read1ByteLe(this->bmpData_ + readOffset + 1, &(pallete[writeOffset].g));
				ByteReader::read1ByteLe(this->bmpData_ + readOffset + 2, &(pallete[writeOffset].r));

				writeOffset++;
				readOffset += 3;
				if (this->palleteByte_ == 4) {
					//palleteByte_が4の場合、予約領域分を読み飛ばし
					readOffset++;
				}
			}
		}

		//パレットBitmap画像からRGBA8888画像へデコード
		void decodeRgba8888FromPalleteBitmap(std::uint8_t** const outData)
		{
			//パレットデータを取得
			Color pallete[PALLETE_MAXNUM] = { 0 };
			this->getPalleteData(pallete);

			//パディングバイト数を取得
			std::int32_t paddingByte = this->getPaddingByte();

			//出力データへデコード後の画像データを設定
			std::int32_t readOffset = this->imageOffset_;
			for (std::int32_t h = 0; h < this->height_; h++) {
				//一行ずつ処理
				std::int32_t writeOffset = (this->height_ - h - 1) * this->width_ * BYTE_PER_PIXEL_RGBA8888;

				//ビット数に応じたビットオフセットとビットマスク
				std::int32_t bitOfs = 8 - this->bitCount_;
				std::uint8_t bitMask = (0x01 << this->bitCount_) - 1;

				for (std::int32_t w = 0; w < this->width_; w++) {
					//画像データはパレットインデックス
					std::uint8_t index = 0;
					ByteReader::read1ByteLe(this->bmpData_ + readOffset, &index);
					std::int32_t palleteOffset = (index >> bitOfs) & bitMask;

					//出力データへRGBA値を設定
					*((*outData) + writeOffset + 0) = pallete[palleteOffset].r;		//赤
					*((*outData) + writeOffset + 1) = pallete[palleteOffset].g;		//緑
					*((*outData) + writeOffset + 2) = pallete[palleteOffset].b;		//青
					*((*outData) + writeOffset + 3) = 255;

					//ビットオフセットを更新
					bitOfs -= this->bitCount_;
					if (bitOfs < 0) {
						//次の読み込み位置に更新
						bitOfs = 8 - this->bitCount_;
						readOffset++;
					}
					//書き込み位置を更新
					writeOffset += BYTE_PER_PIXEL_RGBA8888;
				}
				readOffset += paddingByte;
			}
		}

		//トゥルーカラーBitmap画像からRGBA8888画像へデコード
		void decodeRgba8888FromTrueColorBitmap(std::uint8_t** const outData)
		{
			//パディングバイト数を取得
			std::int32_t paddingByte = getPaddingByte();

			//出力データへデコード後の画像データを設定
			std::int32_t readOffset = this->imageOffset_;
			for (std::int32_t h = 0; h < this->height_; h++) {
				//一行ずつ処理
				std::int32_t writeIndex = (this->height_ - h - 1) * this->width_ * BYTE_PER_PIXEL_RGBA8888;

				for (std::int32_t w = 0; w < this->width_; w++) {
					//画像データはBGR値
					std::uint8_t b = 255;
					ByteReader::read1ByteLe(this->bmpData_ + readOffset + 0, &b);
					std::uint8_t g = 255;
					ByteReader::read1ByteLe(this->bmpData_ + readOffset + 1, &g);
					std::uint8_t r = 255;
					ByteReader::read1ByteLe(this->bmpData_ + readOffset + 2, &r);
					std::uint8_t a = 255;
					if (this->bitCount_ == 32) {
						//ビット数が32bitの場合、A値を読み込み
						ByteReader::read1ByteLe(this->bmpData_ + readOffset + 3, &a);

						//読み込み位置を更新
						readOffset++;
					}

					//出力データへRGBA値を設定
					*((*outData) + writeIndex + 0) = r;		//赤
					*((*outData) + writeIndex + 1) = g;		//緑
					*((*outData) + writeIndex + 2) = b;		//青
					*((*outData) + writeIndex + 3) = a;		//アルファ

					//読み込み位置を更新
					readOffset += 3;

					//書き込み位置を更新
					writeIndex += BYTE_PER_PIXEL_RGBA8888;
				}
				readOffset += paddingByte;
			}
		}
	};	//Bitmap
}	//namespace


namespace ui {


	//----------------------------------------------------------
	//
	// 画像デコードクラス
	//
	//----------------------------------------------------------

	//コンストラクタ
	ImageDecorder::ImageDecorder() :
		decodeData_(nullptr), decodeSize_(0), width_(0), height_(0)
	{
	}

	//デストラクタ
	ImageDecorder::~ImageDecorder()
	{
		if (this->decodeData_ != nullptr) {
			delete[] this->decodeData_;
		}
	}

	//デコード
	std::int32_t ImageDecorder::decode(const Image& image)
	{
		std::int32_t rc = 0;

		//初期化
		if (this->decodeData_ != nullptr) {
			delete[] this->decodeData_;
			this->decodeData_ = nullptr;
		}
		this->decodeSize_ = int32_t(0);
		this->width_ = int32_t(0);
		this->height_ = int32_t(0);

		//デコード
		ImageIF* bodyIF = nullptr;
		ImageIF* blendIF = nullptr;
		if (image.format_ == EN_ImageFormat::BMP) {
			//BITMAP画像
			bodyIF = new Bitmap(image.body_.data_, image.body_.dataSize_);
			if (image.isBlend_ == 1) {
				blendIF = new Bitmap(image.blend_.data_, image.blend_.dataSize_);
			}
		}
		else if (image.format_ == EN_ImageFormat::PNG) {
			//PNG画像
			bodyIF = new Png(image.body_.data_, image.body_.dataSize_);
			if (image.isBlend_ == 1) {
				blendIF = new Png(image.blend_.data_, image.blend_.dataSize_);
			}
		}
		else {
			//未サポート
			rc = -1;
		}

		if (bodyIF != nullptr) {
			//本体画像をデコード
			this->decodeData_ = bodyIF->decodeRgba8888(&this->decodeSize_, &this->width_, &this->height_);

			delete bodyIF;
		}
		if (blendIF != nullptr) {
			//ブレンド画像をデコード
			std::int32_t blendSize = 0;
			std::int32_t blendWidth = 0;
			std::int32_t blendHeight = 0;
			std::uint8_t* blend = blendIF->decodeRgba8888(&blendSize, &blendWidth, &blendHeight);

			//本体画像へブレンド処理を実施

			delete blendIF;
		}

		return rc;
	}

	//デコードデータ取得
	std::uint8_t* const ImageDecorder::getDecodeData(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height)
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
}
