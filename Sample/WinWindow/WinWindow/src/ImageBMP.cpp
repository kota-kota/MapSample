#include "ImageBMP.hpp"
#include <cstdio>


namespace {

	//RGBA8888画像の1ピクセルあたりのバイト数
	static const std::int32_t BYTE_PER_PIXEL_RGBA8888 = 4;

	//パレット最大数(256色)
	static const std::int32_t PALLETE_MAXNUM = 256;

	//Bitmapファイルヘッダ(Windows,OS/2共通)
	static const std::int32_t BFH_HEADERSIZE = 14;
	static const std::int32_t BFH_FILETYPE_OFS = 0;		//ファイルタイプ
	static const std::int32_t BFH_FILESIZE_OFS = 2;		//ファイルサイズ[byte]
	static const std::int32_t BFH_RESERVED1_OFS = 6;	//予約領域1
	static const std::int32_t BFH_RESERVED2_OFS = 8;	//予約領域2
	static const std::int32_t BFH_IMAGEOFS_OFS = 10;	//ファイル先頭から画像データまでのオフセット[byte]

	//Bitmap情報ヘッダ(Windows)
	static const std::int32_t BIH_HEADERSIZE = 40;
	static const std::int32_t BIH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
	static const std::int32_t BIH_WIDTH_OFS = 18;		//画像の幅[pixel]
	static const std::int32_t BIH_HEIGHT_OFS = 22;		//画像の高さ[pixel]
	static const std::int32_t BIH_PLANENUM_OFS = 26;	//プレーン数
	static const std::int32_t BIH_BITCOUNT_OFS = 28;	//色ビット数[bit]
	static const std::int32_t BIH_COMPRESSION_OFS = 30;	//圧縮形式
	static const std::int32_t BIH_IMGDATASIZE_OFS = 34;	//画像データサイズ[byte]
	static const std::int32_t BIH_XDPM_OFS = 38;		//水平解像度[dot/m]
	static const std::int32_t BIH_YDPM_OFS = 42;		//垂直解像度[dot/m]
	static const std::int32_t BIH_PALLETENUM_OFS = 46;	//パレット数[使用色数]
	static const std::int32_t BIH_IMPCOLORNUM_OFS = 50;	//重要色数
	static const std::int32_t BIH_PALLETE_OFS = 54;		//パレット

	//Bitmapコアヘッダ(OS/2)
	static const std::int32_t BCH_HEADERSIZE = 12;
	static const std::int32_t BCH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
	static const std::int32_t BCH_WIDTH_OFS = 18;		//画像の幅[pixel]
	static const std::int32_t BCH_HEIGHT_OFS = 20;		//画像の高さ[pixel]
	static const std::int32_t BCH_PLANENUM_OFS = 22;	//プレーン数
	static const std::int32_t BCH_BITCOUNT_OFS = 24;	//色ビット数[bit]
	static const std::int32_t BCH_PALLETE_OFS = 26;		//パレット

	//Bitmap圧縮形式
	static const std::uint32_t COMPRESSION_BI_RGB = 0;			//無圧縮
	static const std::uint32_t COMPRESSION_BI_RLE8 = 1;			//ランレングス圧縮[8bpp]
	static const std::uint32_t COMPRESSION_BI_RLE4 = 2;			//ランレングス圧縮[4bpp]
	static const std::uint32_t COMPRESSION_BI_BITFIELDS = 3;	//ビットフィールド

	/**
	 * バイナリ読み込みメソッド群クラス
	 */
	class ByteReader {
	public:
		//1バイトを読み込み(LE)
		static void read1ByteLe(const std::uint8_t* const data, const std::int32_t offset, uint8_t* const readData)
		{
			*readData = *(data + offset + 0);
		}

		//2バイトを読み込み(LE)
		static void read2ByteLe(const std::uint8_t* const data, const std::int32_t offset, uint16_t* const readData)
		{
			*readData = 0;
			uint16_t v1 = uint16_t(*(data + offset + 0));
			*readData |= v1 << 0;
			uint16_t v2 = uint16_t(*(data + offset + 1));
			*readData |= v2 << 8;
		}

		//4バイトを読み込み(LE)
		static void read4ByteLe(const std::uint8_t* const data, const std::int32_t offset, uint32_t* const readData)
		{
			*readData = 0;
			uint32_t v1 = uint32_t(*(data + offset + 0));
			*readData |= v1 << 0;
			uint32_t v2 = uint32_t(*(data + offset + 1));
			*readData |= v2 << 8;
			uint32_t v3 = uint32_t(*(data + offset + 2));
			*readData |= v3 << 16;
			uint32_t v4 = uint32_t(*(data + offset + 3));
			*readData |= v4 << 24;
		}
	};
}	//end of namespace



namespace fw {

	/**
	 * 画像BMPクラス
	 */

	//コンストラクタ
	ImageBMP::ImageBMP() :
		bmp_(nullptr), bmpSize_(0), format_(EN_BmpFormat::INVALID), fileSize_(0), imageOffset_(0),
		width_(0), height_(0), bitCount_(0), compression_(0), imageSize_(0), palleteNum_(0), palleteByte_(0), palleteOffset_(0)
	{
	}

	//デストラクタ
	ImageBMP::~ImageBMP()
	{
	}

	//作成
	std::int32_t ImageBMP::create(std::uint8_t* const bmp, const std::int32_t bmpSize)
	{
		std::int32_t rc = 0;

		//メンバへ保持
		this->bmp_ = bmp;
		this->bmpSize_ = bmpSize;

		//------------------------------------------
		//ファイルヘッダ読み込み

		//ファイルタイプを取得
		std::uint8_t fileType[2];
		ByteReader::read1ByteLe(this->bmp_, BFH_FILETYPE_OFS, &fileType[0]);
		ByteReader::read1ByteLe(this->bmp_, BFH_FILETYPE_OFS + 1, &fileType[1]);
		if ((fileType[0] == 'B') && (fileType[1] == 'M')) {
			//Bitmap画像

			//ファイルサイズを取得
			std::uint32_t fileSize = 0;
			ByteReader::read4ByteLe(this->bmp_, BFH_FILESIZE_OFS, &fileSize);
			this->fileSize_ = fileSize;

			//ファイル先頭から画像データまでのオフセットを取得
			std::uint32_t imageOffset = 0;
			ByteReader::read4ByteLe(this->bmp_, BFH_IMAGEOFS_OFS, &imageOffset);
			this->imageOffset_ = imageOffset;

			//情報ヘッダサイズを取得
			std::uint32_t infoHeaderSize = 0;
			ByteReader::read4ByteLe(this->bmp_, BIH_HEADERSIZE_OFS, &infoHeaderSize);

			//情報ヘッダサイズに応じてフォーマットを選択
			if (infoHeaderSize == BIH_HEADERSIZE) {
				//Windowsフォーマット
				this->format_ = EN_BmpFormat::WINDOWS;

				//Bitmap情報ヘッダ(Windows)読み込み
				rc = this->readInfoHeader_WINDOWS();
			}
			else if (infoHeaderSize == BCH_HEADERSIZE) {
				//OS/2フォーマット
				this->format_ = EN_BmpFormat::OS2;

				//Bitmap情報ヘッダ(OS/2)読み込み
				rc = this->readInfoHeader_OS2();
			}
			else {
				//異常
				rc = -1;
			}
		}
		else {
			//Bitmap画像でない
			rc = -1;
		}

		return rc;
	}

	//幅高さ取得
	void ImageBMP::getWH(std::int32_t* const width, std::int32_t* const height)
	{
		if (width != nullptr) { *width = this->width_; }
		if (height != nullptr) { *height = this->height_; }
	}

	//RGBA8888画像へデコード
	std::int32_t ImageBMP::decode_RGBA8888(std::uint8_t** const decData)
	{
		std::int32_t rc = 0;

		switch (this->bitCount_) {
		case 1:		//1bit
		case 4:		//4bit
		case 8:		//8bit
			rc = this->decodePalleteBitmap_RGBA8888(decData);
			break;
		case 24:	//24bit
		case 32:	//32bit
			rc = this->decodeTrueColorBitmap_RGBA8888(decData);
			break;
		default:
			rc = -1;
			break;
		}

		return rc;
	}

	//Bitmap情報ヘッダ(Windows)読み込み
	std::int32_t ImageBMP::readInfoHeader_WINDOWS()
	{
		//画像の幅と高さを取得
		std::uint32_t width = 0;
		std::uint32_t height = 0;
		ByteReader::read4ByteLe(this->bmp_, BIH_WIDTH_OFS, &width);
		ByteReader::read4ByteLe(this->bmp_, BIH_HEIGHT_OFS, &height);

		//色ビット数を取得
		std::uint16_t bitCount = 0;
		ByteReader::read2ByteLe(this->bmp_, BIH_BITCOUNT_OFS, &bitCount);

		//圧縮形式を取得
		std::uint32_t compression = 0;
		ByteReader::read4ByteLe(this->bmp_, BIH_COMPRESSION_OFS, &compression);

		//画像データサイズを取得
		std::uint32_t imageSize = 0;
		ByteReader::read4ByteLe(this->bmp_, BIH_IMGDATASIZE_OFS, &imageSize);

		//パレット数を取得
		std::uint32_t palleteNum = 0;
		ByteReader::read4ByteLe(this->bmp_, BIH_PALLETENUM_OFS, &palleteNum);
		if ((palleteNum == 0) && (bitCount <= 8)) {
			//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
			palleteNum = 1 << bitCount;
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

		return 0;
	}

	//Bitmap情報ヘッダ(OS/2)読み込み
	std::int32_t ImageBMP::readInfoHeader_OS2()
	{
		//画像の幅と高さを取得
		std::uint16_t width = 0;
		std::uint16_t height = 0;
		ByteReader::read2ByteLe(this->bmp_, BCH_WIDTH_OFS, &width);
		ByteReader::read2ByteLe(this->bmp_, BCH_HEIGHT_OFS, &height);

		//色ビット数を取得
		std::uint16_t bitCount = 0;
		ByteReader::read2ByteLe(this->bmp_, BCH_BITCOUNT_OFS, &bitCount);

		//パレット数を取得
		std::uint32_t palleteNum = 0;
		if (bitCount <= 8) {
			//ビット数が8以下の場合は、パレット数を計算
			palleteNum = 1 << bitCount;
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

		return 0;
	}

	//パディングバイト数を取得
	std::uint32_t ImageBMP::getPaddingByte()
	{
		//画像データサイズ = ((((色ビット数 * 画像の幅) + パティング) × 画像の高さ) / 8)から計算

		//画像データサイズを取得
		std::uint32_t imageSize = this->imageSize_;
		if (imageSize == 0) {
			imageSize = this->fileSize_ - this->imageOffset_;
		}

		//パディングビット数を計算
		std::uint32_t paddingBit = ((imageSize * 8) / this->height_) - (this->width_ * this->bitCount_);

		//パディングバイト数を返す
		return paddingBit / 8;
	}

	//パレットデータを取得
	std::int32_t ImageBMP::getPalleteData(PalColor* const pallete, const std::int32_t numMaxPal)
	{
		std::int32_t rc = 0;

		if(this->palleteNum_ <= numMaxPal) {
			//パレットデータを取得
			std::uint32_t readOffset = this->palleteOffset_;
			std::uint32_t writeOffset = 0;
			for (std::uint16_t p = 0; p < this->palleteNum_; p++) {
				//青→緑→赤
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 0, &(pallete[writeOffset].b));
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 1, &(pallete[writeOffset].g));
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 2, &(pallete[writeOffset].r));

				writeOffset++;
				readOffset += 3;
				if (this->palleteByte_ == 4) {
					//palleteByte_が4の場合、予約領域分を読み飛ばし
					readOffset++;
				}
			}
		}
		else {
			//パレット数異常
			rc = -1;
		}

		return rc;
	}

	//パレットBMP画像からRGBA8888画像へデコード
	std::int32_t ImageBMP::decodePalleteBitmap_RGBA8888(std::uint8_t** const decData)
	{
		std::int32_t rc = 0;

		//パレットデータを取得
		PalColor pallete[PALLETE_MAXNUM] = { 0 };
		rc = this->getPalleteData(pallete, PALLETE_MAXNUM);
		if(rc == 0) {

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
					ByteReader::read1ByteLe(this->bmp_, int16_t(readOffset), &index);
					std::int32_t palleteOffset = (index >> bitOfs) & bitMask;

					//出力データへRGBA値を設定
					*((*decData) + writeOffset + 0) = pallete[palleteOffset].r;		//赤
					*((*decData) + writeOffset + 1) = pallete[palleteOffset].g;		//緑
					*((*decData) + writeOffset + 2) = pallete[palleteOffset].b;		//青
					*((*decData) + writeOffset + 3) = 255;

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
		else {
			//パレット取得失敗
			rc = -1;
		}

		return rc;
	}

	//トゥルーカラーBitmap画像からRGBA8888画像へデコード
	std::int32_t ImageBMP::decodeTrueColorBitmap_RGBA8888(std::uint8_t** const decData)
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
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 0, &b);
				std::uint8_t g = 255;
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 1, &g);
				std::uint8_t r = 255;
				ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 2, &r);
				std::uint8_t a = 255;
				if (this->bitCount_ == 32) {
					//ビット数が32bitの場合、A値を読み込み
					ByteReader::read1ByteLe(this->bmp_, int32_t(readOffset) + 3, &a);

					//読み込み位置を更新
					readOffset++;
				}

				//出力データへRGBA値を設定
				*((*decData) + writeIndex + 0) = r;		//赤
				*((*decData) + writeIndex + 1) = g;		//緑
				*((*decData) + writeIndex + 2) = b;		//青
				*((*decData) + writeIndex + 3) = a;		//アルファ

				//読み込み位置を更新
				readOffset += 3;

				//書き込み位置を更新
				writeIndex += BYTE_PER_PIXEL_RGBA8888;
			}
			readOffset += paddingByte;
		}

		return 0;
	}
}	//end of namespace
