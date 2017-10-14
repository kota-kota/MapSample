﻿#include "Image.hpp"

#include <png.h>
#include <pngstruct.h>

namespace {

	//----------------------------------------------------------
	//
	// PNG画像処理クラス
	//
	//----------------------------------------------------------
	class Png {
		static const std::int32_t PNG_BYTES_TO_CHECK = 4;

		//メンバ変数
		png_bytep	pngData_;		//PNGデータ
		png_structp	pngStr_;		//PNG構造ポインタ(解放必要)
		png_infop	pngInfo_;		//PNG情報ポインタ(解放必要)
		png_uint_32	width_;			//幅
		png_uint_32	height_;		//高さ
		png_size_t	rowByte_;		//行バイト数
		png_byte	bitDepth_;		//ビット深度
		png_byte	colorType_;		//カラータイプ

	public:
		//コンストラクタ
		Png(std::byte_t* pngData) :
			pngData_(pngData), pngStr_(nullptr), pngInfo_(nullptr),
			width_(0), height_(0), rowByte_(0), bitDepth_(0), colorType_(0)
		{
			//PNG初期化処理
			InitializePng();
		}

		//デストラクタ
		~Png()
		{
			//PNG終了処理
			FinalizePng();
		}

		//幅高さを取得
		void GetWH(std::uint32_t* const width, std::uint32_t* const height)
		{
			*width = this->width_;
			*height = this->height_;
		}

		//RGBA8888画像へデコード
		void DecodeRgba8888(std::byte_t** const outData)
		{
			//デコード後の画像データを格納するメモリを確保(本関数の最後に解放)
			png_size_t pngSize = this->height_ * sizeof(png_bytep) + this->height_ * this->rowByte_;
			png_byte* tmp = new png_byte[pngSize];
			png_bytepp png = (png_bytepp)tmp;

			//png_read_imageには各行へのポインタを渡すため、2次元配列化
			png_bytep wp = (png_bytep)&png[this->height_];
			for (png_uint_32 h = 0; h < this->height_; h++) {
				png[h] = wp;
				wp += this->rowByte_;
			}

			//PNGイメージ読み込み
			png_read_image(this->pngStr_, png);

			switch (this->colorType_) {
			case PNG_COLOR_TYPE_GRAY:		//0:グレー
				DecodeRgba8888FromGrayScalePng(outData, png);
				break;
			case PNG_COLOR_TYPE_RGB:		//2:トゥルーカラー
				DecodeRgba8888FromTrueColorPng(outData, png);
				break;
			case PNG_COLOR_TYPE_PALETTE:	//3:パレット
				DecodeRgba8888FromPalletePng(outData, png);
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
		}

	private:
		//PNG読み込みコールバック
		static void CallBackReadPng(png_structp pngStr, png_bytep data, png_size_t length)
		{
			png_bytepp buf = (png_bytepp)png_get_io_ptr(pngStr);
			memcpy_s(data, length, *buf, length);
			*buf += length;
		}

		//PNG初期化処理
		void InitializePng()
		{
			//PNGシグネチャのチェック
			png_byte sig[PNG_BYTES_TO_CHECK];
			(void)memcpy_s(sig, PNG_BYTES_TO_CHECK, this->pngData_, PNG_BYTES_TO_CHECK);
			if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK) == 0) {
				//PNG画像

				//PNG構造ポインタ作成
				this->pngStr_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

				//PNG情報ポインタ作成
				this->pngInfo_ = png_create_info_struct(this->pngStr_);

				//シグネチャ読み込み済み
				png_set_sig_bytes(this->pngStr_, PNG_BYTES_TO_CHECK);
				this->pngData_ += PNG_BYTES_TO_CHECK;

				//PNG読み込みコールバック関数を登録
				png_set_read_fn(this->pngStr_, &this->pngData_, CallBackReadPng);

				//PNG読み込み
				png_read_info(this->pngStr_, this->pngInfo_);

				//IHDRチャンクの各種情報取得
				this->height_ = png_get_image_height(this->pngStr_, this->pngInfo_);
				this->width_ = png_get_image_width(this->pngStr_, this->pngInfo_);
				this->rowByte_ = png_get_rowbytes(this->pngStr_, this->pngInfo_);
				this->bitDepth_ = png_get_bit_depth(this->pngStr_, this->pngInfo_);
				this->colorType_ = png_get_color_type(this->pngStr_, this->pngInfo_);
			}
			else {
				//PNG画像でない
			}
		}

		//PNG終了処理
		void FinalizePng()
		{
			if (this->pngInfo_ != nullptr) {
				png_destroy_info_struct(this->pngStr_, &this->pngInfo_);
			}
			if (this->pngStr_ != nullptr) {
				png_destroy_read_struct(&this->pngStr_, nullptr, nullptr);
			}
		}

		//グレーPNG画像からRGBA8888画像へデコード
		void DecodeRgba8888FromGrayScalePng(std::byte_t** const outData, const png_bytepp png)
		{
			if (this->bitDepth_ <= 8) {
				//ビット深度が1bit,2bit,4bit,8bitの場合

				//ビット深度で表現できる最大値
				png_byte bitMaxValue = (0x01 << this->bitDepth_) - 1;

				//グレーサンプル値(輝度に応じたグレーカラー取得に必要)
				png_byte graySample = 255 / bitMaxValue;

				//出力データへデコード後の画像データを設定
				for (png_uint_32 h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					png_uint_32 writeOffset = h * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
					png_uint_32 readOffset = 0;

					//ビット深度に応じたビットオフセット
					png_int_16 bitOfs = 8 - this->bitDepth_;

					for (png_uint_32 w = 0; w < this->width_; w++) {
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
						writeOffset += fw::BYTE_PER_PIXEL_RGBA8888;
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
		void DecodeRgba8888FromTrueColorPng(std::byte_t** const outData, const png_bytepp png)
		{
			if (this->bitDepth_ == 8) {
				//ビット深度が8bitの場合

				//出力データへデコード後の画像データを設定
				for (png_uint_32 h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					png_uint_32 writeOffset = h * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
					png_uint_32 readOffset = 0;

					for (png_uint_32 w = 0; w < this->width_; w++) {
						//出力データへRGBA値を設定
						*((*outData) + writeOffset + 0) = wp[readOffset + 0];
						*((*outData) + writeOffset + 1) = wp[readOffset + 1];
						*((*outData) + writeOffset + 2) = wp[readOffset + 2];
						*((*outData) + writeOffset + 3) = 255;

						//書き込み位置を更新
						writeOffset += fw::BYTE_PER_PIXEL_RGBA8888;
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
		void DecodeRgba8888FromPalletePng(std::byte_t** const outData, const png_bytepp png)
		{
			//PLTEチャンク読み込み
			png_colorp pallete = nullptr;
			std::int32_t palleteNum = 0;
			png_get_PLTE(this->pngStr_, this->pngInfo_, &pallete, &palleteNum);

			if ((pallete != nullptr) && (palleteNum > 0)) {
				//パレットデータ取得成功

				//出力データへデコード後の画像データを設定
				for (png_uint_32 h = 0; h < this->height_; h++) {
					//一行ずつ処理
					png_bytep wp = png[h];

					//書き込み位置と読み込み位置を初期化
					png_uint_32 writeOffset = h * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
					png_uint_32 readOffset = 0;

					//ビット深度に応じたビットオフセットとビットマスク
					png_int_16 bitOfs = 8 - this->bitDepth_;
					png_byte bitMask = (0x01 << this->bitDepth_) - 1;

					for (png_uint_32 w = 0; w < this->width_; w++) {
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
						writeOffset += fw::BYTE_PER_PIXEL_RGBA8888;
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
	// Bitmap画像処理クラス
	//
	//----------------------------------------------------------
	class Bitmap {

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
		enum BitmapFormat {
			INVALID,		//無効
			WINDOWS,		//Windowsフォーマット
			OS2,			//OS/2フォーマット
		};

		//メンバ変数
		std::byte_t*	bmpData_;		//Bitmapデータ
		BitmapFormat	format_;		//Bitmapフォーマットタイプ
		std::uint32_t	fileSize_;		//ファイルサイズ
		std::uint32_t	imageOffset_;	//画像データまでのオフセット
		std::uint32_t	width_;			//幅
		std::uint32_t	height_;		//高さ
		std::uint32_t	bitCount_;		//ビット数
		std::uint32_t	compression_;	//圧縮形式
		std::uint32_t	imageSize_;		//画像データサイズ
		std::uint32_t	palleteNum_;	//パレット数
		std::uint32_t	palleteByte_;	//1パレットあたりのバイト数
		std::uint32_t	palleteOffset_;	//パレットまでのオフセット

	public:
		//コンストラクタ
		Bitmap(std::byte_t* bmpData) :
			bmpData_(bmpData), format_(BitmapFormat::INVALID), fileSize_(0), imageOffset_(0),
			width_(0), height_(0), bitCount_(0), compression_(0), imageSize_(0), palleteNum_(0), palleteByte_(0), palleteOffset_(0)
		{
			//Bitmapヘッダ読み込み
			ReadHeader();
		}

		//幅高さを取得
		void GetWH(std::uint32_t* const width, std::uint32_t* const height)
		{
			*width = this->width_;
			*height = this->height_;
		}

		//RGBA8888画像へデコード
		void DecodeRgba8888(std::byte_t** const outData)
		{
			switch (this->bitCount_) {
			case 1:		//1bit
				DecodeRgba8888From1BitBitmap(outData);
				break;
			case 4:		//4bit
				DecodeRgba8888From4BitBitmap(outData);
				break;
			case 8:		//8bit
				DecodeRgba8888From8BitBitmap(outData);
				break;
			case 24:	//24bit
				DecodeRgba8888From24BitBitmap(outData);
				break;
			case 32:	//32bit
				DecodeRgba8888From32BitBitmap(outData);
				break;
			default:
				break;
			}
		}

	private:
		//Bitmapヘッダ読み込み
		void ReadHeader()
		{
			//ファイルヘッダ読み込み

			//ファイルタイプを取得
			std::uint8_t fileType[2];
			std::ByteReader::read1ByteLe(this->bmpData_ + BFH_FILETYPE_OFS, &fileType[0]);
			std::ByteReader::read1ByteLe(this->bmpData_ + BFH_FILETYPE_OFS + 1, &fileType[1]);
			if ((fileType[0] == 'B') && (fileType[1] == 'M')) {
				//Bitmap画像

				//ファイルサイズを取得
				std::ByteReader::read4ByteLe(this->bmpData_ + BFH_FILESIZE_OFS, &this->fileSize_);

				//ファイル先頭から画像データまでのオフセットを取得
				std::ByteReader::read4ByteLe(this->bmpData_ + BFH_IMAGEOFS_OFS, &this->imageOffset_);

				//情報ヘッダサイズを取得
				std::uint32_t infoHeaderSize;
				std::ByteReader::read4ByteLe(this->bmpData_ + BIH_HEADERSIZE_OFS, &infoHeaderSize);

				//情報ヘッダサイズに応じてフォーマットタイプを選択
				if (infoHeaderSize == BIH_HEADERSIZE) {
					//Windowsフォーマット
					this->format_ = WINDOWS;
					//Bitmap情報ヘッダ(Windows)読み込み
					ReadInfoHeaderWindows();
				}
				else if (infoHeaderSize == BCH_HEADERSIZE) {
					//OS/2フォーマット
					this->format_ = OS2;
					//Bitmap情報ヘッダ(OS/2)読み込み
					ReadInfoHeaderOS2();
				}
				else {
					//異常
					this->format_ = INVALID;
				}
			}
		}

		//Bitmap情報ヘッダ(Windows)読み込み
		void ReadInfoHeaderWindows()
		{
			//画像の幅と高さを取得
			std::uint32_t width, height;
			std::ByteReader::read4ByteLe(this->bmpData_ + BIH_WIDTH_OFS, &width);
			std::ByteReader::read4ByteLe(this->bmpData_ + BIH_HEIGHT_OFS, &height);

			//色ビット数を取得
			std::uint16_t bitCount;
			std::ByteReader::read2ByteLe(this->bmpData_ + BIH_BITCOUNT_OFS, &bitCount);

			//圧縮形式を取得
			std::uint32_t compression;
			std::ByteReader::read4ByteLe(this->bmpData_ + BIH_COMPRESSION_OFS, &compression);

			//画像データサイズを取得
			std::uint32_t imageSize;
			std::ByteReader::read4ByteLe(this->bmpData_ + BIH_IMGDATASIZE_OFS, &imageSize);

			//パレット数を取得
			std::uint32_t palleteNum;
			std::ByteReader::read4ByteLe(this->bmpData_ + BIH_PALLETENUM_OFS, &palleteNum);
			if ((palleteNum == 0) && (bitCount <= 8)) {
				//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
				palleteNum = (1 << bitCount);
			}

			//メンバ変数に設定
			this->width_ = width;
			this->height_ = height;
			this->bitCount_ = uint32_t(bitCount);
			this->compression_ = compression;
			this->imageSize_ = imageSize;
			this->palleteNum_ = palleteNum;
			this->palleteByte_ = 4;
			this->palleteOffset_ = uint32_t(BIH_PALLETE_OFS);
		}

		//Bitmap情報ヘッダ(OS/2)読み込み
		void ReadInfoHeaderOS2()
		{
			//画像の幅と高さを取得
			std::uint16_t width, height;
			std::ByteReader::read2ByteLe(this->bmpData_ + BCH_WIDTH_OFS, &width);
			std::ByteReader::read2ByteLe(this->bmpData_ + BCH_HEIGHT_OFS, &height);

			//色ビット数を取得
			std::uint16_t bitCount;
			std::ByteReader::read2ByteLe(this->bmpData_ + BCH_BITCOUNT_OFS, &bitCount);

			//パレット数を取得
			std::uint32_t palleteNum = 0;
			if (bitCount <= 8) {
				//ビット数が8以下の場合は、パレット数を計算
				palleteNum = (1 << bitCount);
			}

			//メンバ変数に設定
			this->width_ = uint32_t(width);
			this->height_ = uint32_t(height);
			this->bitCount_ = uint32_t(bitCount);
			this->compression_ = uint32_t(0);
			this->imageSize_ = uint32_t(0);
			this->palleteNum_ = palleteNum;
			this->palleteByte_ = 3;
			this->palleteOffset_ = uint32_t(BCH_PALLETE_OFS);
		}

		//パディングビット数を取得
		//画像データサイズ = ((((色ビット数 * 画像の幅) + パティング) × 画像の高さ) / 8)から計算
		std::uint32_t GetPaddingBit()
		{
			std::uint32_t imageSize = this->imageSize_;
			if (imageSize == 0) {
				imageSize = this->fileSize_ - this->imageOffset_;
			}

			return ((imageSize * 8) / this->height_) - (this->width_ * this->bitCount_);
		}

		//パディングバイト数を取得
		std::uint32_t GetPaddingByte()
		{
			return GetPaddingBit() / 8;
		}

		//パレットデータを取得
		void GetPalleteData(std::byte_t* const pallete)
		{
			//パレットデータを取得
			std::uint32_t readIndex = this->palleteOffset_;
			std::uint32_t writeIndex = 0;
			for (std::uint16_t i = 0; i < this->palleteNum_; i++) {
				for (std::uint32_t j = 0; j < this->palleteByte_; j++) {
					//palleteByte_が3の場合、青→緑→赤
					//palleteByte_が4の場合、青→緑→赤→予約領域
					std::ByteReader::read1ByteLe(this->bmpData_ + readIndex, pallete + writeIndex);

					readIndex++;
					writeIndex++;
				}
			}
		}

		//1BitBitmap画像からRGBA8888画像へデコード
		void DecodeRgba8888From1BitBitmap(std::byte_t** const outData)
		{
			//パレットデータを取得
			std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
			this->GetPalleteData(pallete);

			//パディングバイト数を取得
			std::uint32_t paddingByte = this->GetPaddingByte();

			//出力データを取得
			std::uint32_t imageOffset = this->imageOffset_;
			for (std::uint32_t h = 0; h < this->height_; h++) {
				std::uint32_t writeIndex = (this->height_ - h - 1) * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
				std::uint8_t index = 0;
				std::uint8_t readBit = 7;
				for (std::uint32_t w = 0; w < this->width_; w++) {
					//画像データ取得

					//画像データはパレットのインデックス
					if ((w % 8) == 0) {
						std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset, &index);
						readBit = 7;
						imageOffset++;
					}

					int32_t palleteIndex = ((index >> readBit) & 0x01) * this->palleteByte_;
					*((*outData) + writeIndex + 0) = *(pallete + palleteIndex + 2);		//赤
					*((*outData) + writeIndex + 1) = *(pallete + palleteIndex + 1);		//緑
					*((*outData) + writeIndex + 2) = *(pallete + palleteIndex + 0);		//青
					*((*outData) + writeIndex + 3) = 255;

					readBit--;
					writeIndex += fw::BYTE_PER_PIXEL_RGBA8888;
				}
				imageOffset += paddingByte;
			}
		}

		//4BitBitmap画像からRGBA8888画像へデコード
		void DecodeRgba8888From4BitBitmap(std::byte_t** const outData)
		{
			//パレットデータを取得
			std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
			this->GetPalleteData(pallete);

			//パディングバイト数を取得
			std::uint32_t paddingByte = this->GetPaddingByte();

			//出力データを取得
			std::uint32_t imageOffset = this->imageOffset_;
			for (std::uint32_t h = 0; h < this->height_; h++) {
				std::uint32_t writeIndex = (this->height_ - h - 1) * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
				std::uint8_t index = 0;
				std::uint8_t readBit = 4;
				for (std::uint32_t w = 0; w < this->width_; w++) {
					//画像データ取得

					//画像データはパレットのインデックス
					if ((w % 2) == 0) {
						std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset, &index);
						readBit = 4;
						imageOffset++;
					}

					int32_t palleteIndex = ((index >> readBit) & 0x0F) * this->palleteByte_;
					*((*outData) + writeIndex + 0) = *(pallete + palleteIndex + 2);		//赤
					*((*outData) + writeIndex + 1) = *(pallete + palleteIndex + 1);		//緑
					*((*outData) + writeIndex + 2) = *(pallete + palleteIndex + 0);		//青
					*((*outData) + writeIndex + 3) = 255;

					readBit -= 4;
					writeIndex += fw::BYTE_PER_PIXEL_RGBA8888;
				}
				imageOffset += paddingByte;
			}
		}

		//8BitBitmap画像からRGBA8888画像へデコード
		void DecodeRgba8888From8BitBitmap(std::byte_t** const outData)
		{
			//パレットデータを取得
			std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
			this->GetPalleteData(pallete);

			//パディングバイト数を取得
			std::uint32_t paddingByte = this->GetPaddingByte();

			//出力データを取得
			std::uint32_t imageOffset = this->imageOffset_;
			for (std::uint32_t h = 0; h < this->height_; h++) {
				std::uint32_t writeIndex = (this->height_ - h - 1) * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
				for (std::uint32_t w = 0; w < this->width_; w++) {
					//画像データ取得

					//画像データはパレットのインデックス
					std::uint8_t index;
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset, &index);

					int32_t palleteIndex = int32_t(index) * this->palleteByte_;
					*((*outData) + writeIndex + 0) = *(pallete + palleteIndex + 2);		//赤
					*((*outData) + writeIndex + 1) = *(pallete + palleteIndex + 1);		//緑
					*((*outData) + writeIndex + 2) = *(pallete + palleteIndex + 0);		//青
					*((*outData) + writeIndex + 3) = 255;

					imageOffset++;
					writeIndex += fw::BYTE_PER_PIXEL_RGBA8888;
				}
				imageOffset += paddingByte;
			}
		}

		//24BitBitmap画像からRGBA8888画像へデコード
		void DecodeRgba8888From24BitBitmap(std::byte_t** const outData)
		{
			//パディングバイト数を取得
			std::uint32_t paddingByte = GetPaddingByte();

			//出力データを取得
			std::uint32_t imageOffset = this->imageOffset_;
			for (std::uint32_t h = 0; h < this->height_; h++) {
				std::uint32_t writeIndex = (this->height_ - h - 1) * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
				for (std::uint32_t w = 0; w < this->width_; w++) {
					//画像データ取得

					std::uint8_t b, g, r;
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 0, &b);
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 1, &g);
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 2, &r);

					*((*outData) + writeIndex + 0) = r;		//赤
					*((*outData) + writeIndex + 1) = g;		//緑
					*((*outData) + writeIndex + 2) = b;		//青
					*((*outData) + writeIndex + 3) = 255;

					imageOffset += 3;
					writeIndex += fw::BYTE_PER_PIXEL_RGBA8888;
				}
				imageOffset += paddingByte;
			}
		}

		//32BitBitmap画像からRGBA8888画像へデコード
		void DecodeRgba8888From32BitBitmap(std::byte_t** const outData)
		{
			//パディングバイト数を取得
			std::uint32_t paddingByte = this->GetPaddingByte();

			//出力データを取得
			std::uint32_t imageOffset = this->imageOffset_;
			for (std::uint32_t h = 0; h < this->height_; h++) {
				std::uint32_t writeIndex = (this->height_ - h - 1) * this->width_ * fw::BYTE_PER_PIXEL_RGBA8888;
				for (std::uint32_t w = 0; w < this->width_; w++) {
					//画像データ取得

					std::uint8_t b, g, r, a;
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 0, &b);
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 1, &g);
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 2, &r);
					std::ByteReader::read1ByteLe(this->bmpData_ + imageOffset + 3, &a);

					*((*outData) + writeIndex + 0) = r;		//赤
					*((*outData) + writeIndex + 1) = g;		//緑
					*((*outData) + writeIndex + 2) = b;		//青
					*((*outData) + writeIndex + 3) = 255;

					imageOffset += 4;
					writeIndex += fw::BYTE_PER_PIXEL_RGBA8888;
				}
				imageOffset += paddingByte;
			}
		}
	};	//Bitmap
}	//namespace


//----------------------------------------------------------
//
// 画像クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::Image::Image() :
	id_(0), type_(ImageType::UNKNOWN), format_(ImageFormat::UNKNOWN), width_(0), height_(0), transColor_(),
	bodySize_(0), body_(nullptr), blendSize_(0), blend_(nullptr), extPalleteNum_(0), extPallete_(nullptr)
{
}
fw::Image::Image(const std::uint32_t id, const ImageType type, const ImageFormat format) :
	id_(id), type_(type), format_(format), width_(0), height_(0), transColor_(),
	bodySize_(0), body_(nullptr), blendSize_(0), blend_(nullptr), extPalleteNum_(0), extPallete_(nullptr)
{
}

//デストラクタ
fw::Image::~Image()
{
	this->Free();
}

//本体画像作成(バイナリデータから)
bool fw::Image::Create(const std::byte_t* const body, const size_t bodySize, const std::uint32_t width, const std::uint32_t height)
{
	bool success = false;

	//本体画像作成実施判定
	bool isInit = this->IsInit();
	bool isCreate = this->IsCreate();
	if ((isInit) && (!isCreate)) {
		//初期化済みかつ未作成の場合のみ実施する

		//本体画像を生成(メモリ確保してコピー)
		this->body_ = new std::byte_t[bodySize];
		(void)memcpy_s(this->body_, bodySize, body, bodySize);

		//本体画像データサイズを設定
		this->bodySize_ = bodySize;

		//画像幅高さを設定
		std::uint32_t w = width;
		std::uint32_t h = height;
		if ((w == 0) || (h == 0)) {
			//幅高さ指定がないので取得
			this->GetWH(&w, &h);
		}
		this->width_ = w;
		this->height_ = h;

		success = true;
	}
	return success;
}

//本体画像作成(ファイルから)
bool fw::Image::Create(const std::string bodyFile, const std::uint32_t width = 0, const std::uint32_t height = 0)
{
	bool success = false;

	//本体画像ファイルオープン
	FILE* fp;
	(void)fopen_s(&fp, bodyFile.c_str(), "rb");
	if (fp != nullptr) {
		//オープン成功

		//本体画像データサイズ取得
		fpos_t pos;
		(void)fseek(fp, 0, SEEK_END);
		(void)fgetpos(fp, &pos);
		(void)fseek(fp, 0, SEEK_SET);
		size_t bodySize = size_t(pos);

		//本体画像データのメモリ確保
		std::byte_t* body = new std::byte_t[bodySize];

		//本体画像データ読み込み
		size_t readSize = fread_s(body, bodySize, 1, bodySize, fp);
		if (readSize == bodySize) {
			//読み込み成功

			//バイナリデータから本体画像作成
			success = this->Create(body, bodySize, width, height);
		}

		//本体画像データのメモリ解放
		delete[] body;

		//ファイルクローズ
		fclose(fp);
	}

	return success;
}

//解放
void fw::Image::Free()
{
	this->id_ = 0;
	this->type_ = ImageType::UNKNOWN;
	this->format_ = ImageFormat::UNKNOWN;
	this->width_ = 0;
	this->height_ = 0;
	this->transColor_ = std::Color();
	this->bodySize_ = 0;
	if (this->body_ != nullptr) {
		delete[] this->body_;
		this->body_ = nullptr;
	}
	this->blendSize_ = 0;
	if (this->blend_ != nullptr) {
		delete[] this->blend_;
		this->blend_ = nullptr;
	}
	this->extPalleteNum_ = 0;
	if (this->extPallete_ != nullptr) {
		delete[] this->extPallete_;
		this->extPallete_ = nullptr;
	}
}

//ブレンド画像設定(バイナリデータから)
bool fw::Image::SetBlend(const std::byte_t* const blend, const size_t blendSize)
{
	bool success = false;

	//ブレンド画像設定実施判定
	bool isCreate = this->IsCreate();
	if (isCreate) {
		//作成済みの場合のみ実施する

		//ブレンド画像を生成(メモリ確保してコピー)
		this->blend_ = new std::byte_t[blendSize];
		(void)memcpy_s(this->blend_, blendSize, blend, blendSize);

		//ブレンド画像データサイズを設定
		this->blendSize_ = blendSize;

		success = true;
	}
	return success;
}

//ブレンド画像設定(ファイルから)
bool fw::Image::SetBlend(const std::string blendFile)
{
	bool success = false;

	//ブレンド画像ファイルオープン
	FILE* fp;
	(void)fopen_s(&fp, blendFile.c_str(), "rb");
	if (fp != nullptr) {
		//オープン成功

		//ブレンド画像データサイズ取得
		fpos_t pos;
		(void)fseek(fp, 0, SEEK_END);
		(void)fgetpos(fp, &pos);
		(void)fseek(fp, 0, SEEK_SET);
		size_t blendSize = size_t(pos);

		//ブレンド画像データのメモリ確保
		std::byte_t* blend = new std::byte_t[blendSize];

		//ブレンド画像データ読み込み
		size_t readSize = fread_s(blend, blendSize, 1, blendSize, fp);
		if (readSize == blendSize) {
			//読み込み成功

			//バイナリデータからブレンド画像設定
			success = this->Create(blend, blendSize, 0, 0);
		}

		//ブレンド画像データのメモリ解放
		delete[] blend;

		//ファイルクローズ
		fclose(fp);
	}

	return success;
}

//透過色設定
bool fw::Image::SetTransColor(const std::Color transColor)
{
	bool success = false;

	//透過色設定実施判定
	bool isCreate = this->IsCreate();
	if (isCreate) {
		//作成済みの場合のみ実施する

		//透過を色設定
		this->transColor_ = transColor;

		success = true;
	}
	return success;
}

//拡張パレット設定
bool fw::Image::SetExtPallete(const std::Color* const pallete, const std::uint32_t palleteNum)
{
	bool success = false;

	//拡張パレット設定実施判定
	bool isCreate = this->IsCreate();
	if (isCreate) {
		//作成済みの場合のみ実施する

		//拡張パレットを生成(メモリ確保してコピー)
		this->extPallete_ = new std::Color[palleteNum];
		const size_t palleteSize = sizeof(std::Color) * palleteNum;
		(void)memcpy_s(this->extPallete_, palleteSize, pallete, palleteSize);

		//拡張パレット数を設定
		this->extPalleteNum_ = palleteNum;

		success = true;
	}
	return success;
}

//初期化済み判定
bool fw::Image::IsInit()
{
	bool isInit = true;
	if ((this->id_ == 0) ||
		(this->type_ == ImageType::UNKNOWN) ||
		(this->format_ == ImageFormat::UNKNOWN)) {
		isInit = false; 
	}
	return isInit;
}

//作成済み判定
bool fw::Image::IsCreate()
{
	bool isCreate = true;
	if ((this->bodySize_ <= 0) ||
		(this->body_ == nullptr)) {
		isCreate = false;
	}
	return isCreate;
}

//幅高さを取得
void fw::Image::GetWH(std::uint32_t* const width, std::uint32_t* const height)
{
	if (this->format_ == ImageFormat::BMP) {
		//BMP画像
		Bitmap bitmap(this->body_);
		bitmap.GetWH(width, height);
	}
	else if (this->format_ == ImageFormat::PNG) {
		//PNG画像
		Png png(this->body_);
		png.GetWH(width, height);
	}
	else {
		//上記フォーマット以外は未サポート
	}
}

//デコード(RGBA8888)
void fw::Image::DecodeRgba8888(Image* const outImage)
{
	//RGBA8888画像の1ピクセルあたりのバイト数
	const uint32_t BYTE_PER_PIXEL = 4;

	//デコード後のデータサイズ
	std::uint32_t dataSize = this->width_ * this->height_ * BYTE_PER_PIXEL;

	//本体画像デコード
	std::byte_t* bodyData = nullptr;
	if (this->body_ != nullptr) {
		//デコード後のRGBA8888画像のメモリ確保
		bodyData = new std::byte_t[dataSize];

		//画像フォーマットに応じたデコード処理
		if (this->format_ == ImageFormat::BMP) {
			Bitmap bitmap(this->body_);
			bitmap.DecodeRgba8888(&bodyData);
		}
		else if (this->format_ == ImageFormat::PNG) {
			Png png(this->body_);
			png.DecodeRgba8888(&bodyData);
		}
		else {
			//ここには来ないはず
		}
	}

	//ブレンド画像デコード
	std::byte_t* blendData = nullptr;
	if (this->blend_ != nullptr) {
		//デコード後のRGBA8888画像のメモリ確保
		blendData = new std::byte_t[dataSize];

		//画像フォーマットに応じたデコード処理
		if (this->format_ == ImageFormat::BMP) {
			Bitmap bitmap(this->blend_);
			bitmap.DecodeRgba8888(&blendData);
		}
		else if (this->format_ == ImageFormat::PNG) {
			Png png(this->blend_);
			png.DecodeRgba8888(&bodyData);
		}
		else {
			//ここには来ないはず
		}
	}

	//本体画像とブレンド画像が両方あればブレンド処理
	if ((bodyData != nullptr) && (blendData != nullptr)) {
	}

	//出力するRGBA8888画像オブジェクトを作成
	new(outImage) Image(this->id_, this->type_, ImageFormat::RGBA8888);
	outImage->Create(bodyData, dataSize, this->width_, this->height_);

	//確保したメモリを解放
	if (bodyData != nullptr) {
		delete[] bodyData;
	}
	if (blendData != nullptr) {
		delete[] blendData;
	}
}
