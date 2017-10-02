#include "Image.hpp"


//----------------------------------------------------------
//
// Bitmap画像処理
//
//----------------------------------------------------------

namespace
{
	//----------------------------------------------------------
	//
	// Bitmap画像処理用定義
	//
	//----------------------------------------------------------

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

	//Bitmapファイルヘッダ構造体
	struct FileHeader {
		BitmapFormat	format_;		//Bitmapフォーマットタイプ
		std::uint32_t	fileSize_;		//ファイルサイズ
		std::uint32_t	imageOffset_;	//画像データまでのオフセット
	};

	//Bitmap情報ヘッダ構造体(Windows,OS/2共通)
	struct InfoHeader {
		std::uint32_t	width_;				//幅
		std::uint32_t	height_;			//高さ
		std::uint32_t	bitCount_;			//ビット数
		std::uint32_t	compression_;		//圧縮形式
		std::uint32_t	imageSize_;			//画像データサイズ
		std::uint32_t	palleteNum_;		//パレット数
		std::uint32_t	palleteByte_;		//1パレットあたりのバイト数
		std::uint32_t	palleteOffset_;		//パレットまでのオフセット
	};

	//----------------------------------------------------------
	//
	// Bitmap画像処理内部関数
	//
	//----------------------------------------------------------

	//Bitmapファイルヘッダ読み込み
	static void ReadFileHeader(const std::byte_t* const bmpData, FileHeader* const fileHeader)
	{
		BitmapFormat format = INVALID;
		std::uint32_t fileSize = 0;
		std::uint32_t imageOffset = 0;

		//ファイルタイプを取得
		std::uint8_t fileType[2];
		std::ByteReader::read1ByteLe(bmpData + BFH_FILETYPE_OFS, &fileType[0]);
		std::ByteReader::read1ByteLe(bmpData + BFH_FILETYPE_OFS + 1, &fileType[1]);
		if ((fileType[0] == 'B') && (fileType[1] == 'M')) {
			//Bitmap画像

			//ファイルサイズを取得
			std::ByteReader::read4ByteLe(bmpData + BFH_FILESIZE_OFS, &fileSize);

			//ファイル先頭から画像データまでのオフセットを取得
			std::ByteReader::read4ByteLe(bmpData + BFH_IMAGEOFS_OFS, &imageOffset);

			//情報ヘッダサイズを取得
			std::uint32_t infoHeaderSize;
			std::ByteReader::read4ByteLe(bmpData + BIH_HEADERSIZE_OFS, &infoHeaderSize);

			//情報ヘッダサイズに応じてフォーマットタイプを選択
			if (infoHeaderSize == BIH_HEADERSIZE) {
				//Windowsフォーマット
				format = WINDOWS;
			}
			else if (infoHeaderSize == BCH_HEADERSIZE) {
				//OS/2フォーマット
				format = OS2;
			}
			else {
				//異常
				format = INVALID;
			}
		}

		//出力データに設定
		fileHeader->format_ = format;
		fileHeader->fileSize_ = fileSize;
		fileHeader->imageOffset_ = imageOffset;
	}

	//Bitmap情報ヘッダ(Windows)読み込み
	static void ReadInfoHeaderWindows(const std::byte_t* const bmpData, InfoHeader* const infoHeader)
	{
		//画像の幅と高さを取得
		std::uint32_t width, height;
		std::ByteReader::read4ByteLe(bmpData + BIH_WIDTH_OFS, &width);
		std::ByteReader::read4ByteLe(bmpData + BIH_HEIGHT_OFS, &height);

		//色ビット数を取得
		std::uint16_t bitCount;
		std::ByteReader::read2ByteLe(bmpData + BIH_BITCOUNT_OFS, &bitCount);

		//圧縮形式を取得
		std::uint32_t compression;
		std::ByteReader::read4ByteLe(bmpData + BIH_COMPRESSION_OFS, &compression);

		//画像データサイズを取得
		std::uint32_t imageSize;
		std::ByteReader::read4ByteLe(bmpData + BIH_IMGDATASIZE_OFS, &imageSize);

		//パレット数を取得
		std::uint32_t palleteNum;
		std::ByteReader::read4ByteLe(bmpData + BIH_PALLETENUM_OFS, &palleteNum);
		if ((palleteNum == 0) && (bitCount <= 8)) {
			//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
			palleteNum = (1 << bitCount);
		}

		//メンバ変数に設定
		infoHeader->width_ = width;
		infoHeader->height_ = height;
		infoHeader->bitCount_ = uint32_t(bitCount);
		infoHeader->compression_ = compression;
		infoHeader->imageSize_ = imageSize;
		infoHeader->palleteNum_ = palleteNum;
		infoHeader->palleteByte_ = 4;
		infoHeader->palleteOffset_ = uint32_t(BIH_PALLETE_OFS);
	}

	//Bitmap情報ヘッダ(OS/2)読み込み
	static void ReadInfoHeaderOS2(const std::byte_t* const bmpData, InfoHeader* const infoHeader)
	{
		//画像の幅と高さを取得
		std::uint16_t width, height;
		std::ByteReader::read2ByteLe(bmpData + BCH_WIDTH_OFS, &width);
		std::ByteReader::read2ByteLe(bmpData + BCH_HEIGHT_OFS, &height);

		//色ビット数を取得
		std::uint16_t bitCount;
		std::ByteReader::read2ByteLe(bmpData + BCH_BITCOUNT_OFS, &bitCount);

		//パレット数を取得
		std::uint32_t palleteNum = 0;
		if (bitCount <= 8) {
			//ビット数が8以下の場合は、パレット数を計算
			palleteNum = (1 << bitCount);
		}

		//メンバ変数に設定
		infoHeader->width_ = uint32_t(width);
		infoHeader->height_ = uint32_t(height);
		infoHeader->bitCount_ = uint32_t(bitCount);
		infoHeader->compression_ = uint32_t(0);
		infoHeader->imageSize_ = uint32_t(0);
		infoHeader->palleteNum_ = palleteNum;
		infoHeader->palleteByte_ = 3;
		infoHeader->palleteOffset_ = uint32_t(BCH_PALLETE_OFS);
	}

	//Bitmapヘッダ読み込み
	//fileHeader,infoHeaderともにNULL許容(NULLなら出力しない)
	static void ReadHeader(const std::byte_t* const bmpData, FileHeader* const fileHeader, InfoHeader* const infoHeader)
	{
		//Bitmapファイルヘッダ読み込み
		//情報ヘッダ読み込みに必要であるため、出力有無に関わらず読み込む
		FileHeader fh;
		ReadFileHeader(bmpData, &fh);

		if (fileHeader != nullptr) {
			//ファイルヘッダ出力ありなので、出力へコピー
			*fileHeader = fh;
		}

		//Bitmap情報ヘッダ読み込み
		//出力なしの場合は必要ないので読み込まない
		if (infoHeader != nullptr) {
			InfoHeader ih;
			if (fh.format_ == BitmapFormat::WINDOWS) {
				//Bitmap情報ヘッダ(Windows)読み込み
				ReadInfoHeaderWindows(bmpData, &ih);
			}
			else {
				//Bitmap情報ヘッダ(OS/2)読み込み
				ReadInfoHeaderOS2(bmpData, &ih);
			}

			//情報ヘッダ出力ありなので、出力へコピー
			*infoHeader = ih;
		}
	}

	//パディングビット数を取得
	//画像データサイズ = ((((色ビット数 * 画像の幅) + パティング) × 画像の高さ) / 8)から計算
	static std::uint32_t GetPaddingBit(const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		std::uint32_t imageSize = infoHeader.imageSize_;
		if (imageSize == 0) {
			imageSize = fileHeader.fileSize_ - fileHeader.imageOffset_;
		}

		return ((imageSize * 8) / infoHeader.height_) - (infoHeader.width_ * infoHeader.bitCount_);
	}

	//パディングバイト数を取得
	static std::uint32_t GetPaddingByte(const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		return GetPaddingBit(fileHeader, infoHeader) / 8;
	}

	//パレットデータを取得
	static void GetPalleteData(std::byte_t* const pallete, const std::byte_t* const bmpData, const InfoHeader& infoHeader)
	{
		//パレットデータを取得
		std::uint32_t readIndex = infoHeader.palleteOffset_;
		std::uint32_t writeIndex = 0;
		for (std::uint16_t i = 0; i < infoHeader.palleteNum_; i++) {
			for (std::uint32_t j = 0; j < infoHeader.palleteByte_; j++) {
				//palleteByte_が3の場合、青→緑→赤
				//palleteByte_が4の場合、青→緑→赤→予約領域
				std::ByteReader::read1ByteLe(bmpData + readIndex, pallete + writeIndex);

				readIndex++;
				writeIndex++;
			}
		}
	}

	//1BitBitmap画像からからRGBA8888画像へデコード
	static void DecodeRgba8888From1BitBitmap(std::byte_t** const outData, const std::byte_t* const bmpData, const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		//パレットデータを取得
		std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
		GetPalleteData(pallete, bmpData, infoHeader);

		//パディングバイト数を取得
		std::uint32_t paddingByte = GetPaddingByte(fileHeader, infoHeader);

		//出力データを取得
		std::uint32_t imageOffset = fileHeader.imageOffset_;
		for (std::uint32_t h = 0; h < infoHeader.height_; h++) {
			std::uint32_t writeIndex = (infoHeader.height_ - h - 1) * infoHeader.width_ * fw::BYTE_PER_PIXEL_RGBA8888;
			std::uint8_t index = 0;
			std::uint8_t readBit = 7;
			for (std::uint32_t w = 0; w < infoHeader.width_; w++) {
				//画像データ取得

				//画像データはパレットのインデックス
				if ((w % 8) == 0) {
					std::ByteReader::read1ByteLe(bmpData + imageOffset, &index);
					readBit = 7;
					imageOffset++;
				}

				int32_t palleteIndex = ((index >> readBit) & 0x01) * infoHeader.palleteByte_;
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

	//4BitBitmap画像からからRGBA8888画像へデコード
	static void DecodeRgba8888From4BitBitmap(std::byte_t** const outData, const std::byte_t* const bmpData, const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		//パレットデータを取得
		std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
		GetPalleteData(pallete, bmpData, infoHeader);

		//パディングバイト数を取得
		std::uint32_t paddingByte = GetPaddingByte(fileHeader, infoHeader);

		//出力データを取得
		std::uint32_t imageOffset = fileHeader.imageOffset_;
		for (std::uint32_t h = 0; h < infoHeader.height_; h++) {
			std::uint32_t writeIndex = (infoHeader.height_ - h - 1) * infoHeader.width_ * fw::BYTE_PER_PIXEL_RGBA8888;
			std::uint8_t index = 0;
			std::uint8_t readBit = 4;
			for (std::uint32_t w = 0; w < infoHeader.width_; w++) {
				//画像データ取得

				//画像データはパレットのインデックス
				if ((w % 2) == 0) {
					std::ByteReader::read1ByteLe(bmpData + imageOffset, &index);
					readBit = 4;
					imageOffset++;
				}

				int32_t palleteIndex = ((index >> readBit) & 0x0F) * infoHeader.palleteByte_;
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

	//8BitBitmap画像からからRGBA8888画像へデコード
	static void DecodeRgba8888From8BitBitmap(std::byte_t** const outData, const std::byte_t* const bmpData, const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		//パレットデータを取得
		std::byte_t pallete[fw::PALLETE_MAXSIZE] = { 0 };
		GetPalleteData(pallete, bmpData, infoHeader);

		//パディングバイト数を取得
		std::uint32_t paddingByte = GetPaddingByte(fileHeader, infoHeader);

		//出力データを取得
		std::uint32_t imageOffset = fileHeader.imageOffset_;
		for (std::uint32_t h = 0; h < infoHeader.height_; h++) {
			std::uint32_t writeIndex = (infoHeader.height_ - h - 1) * infoHeader.width_ * fw::BYTE_PER_PIXEL_RGBA8888;
			for (std::uint32_t w = 0; w < infoHeader.width_; w++) {
				//画像データ取得

				//画像データはパレットのインデックス
				std::uint8_t index;
				std::ByteReader::read1ByteLe(bmpData + imageOffset, &index);

				int32_t palleteIndex = int32_t(index) * infoHeader.palleteByte_;
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

	//24BitBitmap画像からからRGBA8888画像へデコード
	static void DecodeRgba8888From24BitBitmap(std::byte_t** const outData, const std::byte_t* const bmpData, const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		//パディングバイト数を取得
		std::uint32_t paddingByte = GetPaddingByte(fileHeader, infoHeader);

		//出力データを取得
		std::uint32_t imageOffset = fileHeader.imageOffset_;
		for (std::uint32_t h = 0; h < infoHeader.height_; h++) {
			std::uint32_t writeIndex = (infoHeader.height_ - h - 1) * infoHeader.width_ * fw::BYTE_PER_PIXEL_RGBA8888;
			for (std::uint32_t w = 0; w < infoHeader.width_; w++) {
				//画像データ取得

				std::uint8_t b, g, r;
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 0, &b);
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 1, &g);
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 2, &r);

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

	//32BitBitmap画像からからRGBA8888画像へデコード
	static void DecodeRgba8888From32BitBitmap(std::byte_t** const outData, const std::byte_t* const bmpData, const FileHeader& fileHeader, const InfoHeader& infoHeader)
	{
		//パディングバイト数を取得
		std::uint32_t paddingByte = GetPaddingByte(fileHeader, infoHeader);

		//出力データを取得
		std::uint32_t imageOffset = fileHeader.imageOffset_;
		for (std::uint32_t h = 0; h < infoHeader.height_; h++) {
			std::uint32_t writeIndex = (infoHeader.height_ - h - 1) * infoHeader.width_ * fw::BYTE_PER_PIXEL_RGBA8888;
			for (std::uint32_t w = 0; w < infoHeader.width_; w++) {
				//画像データ取得

				std::uint8_t b, g, r, a;
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 0, &b);
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 1, &g);
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 2, &r);
				std::ByteReader::read1ByteLe(bmpData + imageOffset + 3, &a);

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
}



//----------------------------------------------------------
//
// Bitmap画像処理ライブラリクラス
//
//----------------------------------------------------------

//Bitmap判定
bool fw::ImageLib::Bitmap::IsBitmap(const std::byte_t* const bmpData)
{
	//Bitmapファイルヘッダ読み込み
	FileHeader fileHeader;
	ReadFileHeader(bmpData, &fileHeader);

	bool isBitmap = false;
	if (fileHeader.format_ != BitmapFormat::INVALID) {
		//フォーマットタイプが無効でなければBitmap画像
		isBitmap = true;
	}
	return isBitmap;
}

//幅高さを取得
void fw::ImageLib::Bitmap::GetWH(const std::byte_t* const bmpData, std::uint32_t* const width, std::uint32_t* const height)
{
	//Bitmapヘッダ読み込み
	InfoHeader infoHeader;
	ReadHeader(bmpData, nullptr, &infoHeader);

	//幅高さを出力
	*width = infoHeader.width_;
	*height = infoHeader.height_;
}

//RGBA8888画像へデコード
void fw::ImageLib::Bitmap::DecodeRgba8888(std::byte_t** const outData, const std::byte_t* const bmpData)
{
	//Bitmapヘッダ読み込み
	FileHeader fileHeader;
	InfoHeader infoHeader;
	ReadHeader(bmpData, &fileHeader, &infoHeader);

	switch (infoHeader.bitCount_) {
	case 1:		//1bit
		DecodeRgba8888From1BitBitmap(outData, bmpData, fileHeader, infoHeader);
		break;
	case 4:		//4bit
		DecodeRgba8888From4BitBitmap(outData, bmpData, fileHeader, infoHeader);
		break;
	case 8:		//8bit
		DecodeRgba8888From8BitBitmap(outData, bmpData, fileHeader, infoHeader);
		break;
	case 24:	//24bit
		DecodeRgba8888From24BitBitmap(outData, bmpData, fileHeader, infoHeader);
		break;
	case 32:	//32bit
		DecodeRgba8888From32BitBitmap(outData, bmpData, fileHeader, infoHeader);
		break;
	default:
		break;
	}
}
