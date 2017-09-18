#include "Bitmap.hpp"
#include <new>

namespace fw
{
	//ビットマップファイルヘッダ(Windows,OS/2共通)
	static const std::int16_t BFH_HEADERSIZE = 14;
	static const std::int16_t BFH_FILETYPE_OFS = 0;		//ファイルタイプ
	static const std::int16_t BFH_FILESIZE_OFS = 2;		//ファイルサイズ[byte]
	static const std::int16_t BFH_RESERVED1_OFS = 6;		//予約領域1
	static const std::int16_t BFH_RESERVED2_OFS = 8;		//予約領域2
	static const std::int16_t BFH_IMAGEOFS_OFS = 10;		//ファイル先頭から画像データまでのオフセット[byte]

	//ビットマップ情報ヘッダ(Windows)
	static const std::int16_t BIH_HEADERSIZE = 40;
	static const std::int16_t BIH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
	static const std::int16_t BIH_WIDTH_OFS = 18;		//画像の幅[pixel]
	static const std::int16_t BIH_HEIGHT_OFS = 22;		//画像の高さ[pixel]
	static const std::int16_t BIH_PLANENUM_OFS = 26;		//プレーン数
	static const std::int16_t BIH_BITCOUNT_OFS = 28;		//色ビット数[bit]
	static const std::int16_t BIH_COMPRESSION_OFS = 30;	//圧縮形式
	static const std::int16_t BIH_IMGDATASIZE_OFS = 34;	//画像データサイズ[byte]
	static const std::int16_t BIH_XDPM_OFS = 38;			//水平解像度[dot/m]
	static const std::int16_t BIH_YDPM_OFS = 42;			//垂直解像度[dot/m]
	static const std::int16_t BIH_PALLETENUM_OFS = 46;	//パレット数[使用色数]
	static const std::int16_t BIH_IMPCOLORNUM_OFS = 50;	//重要色数
	static const std::int16_t BIH_PALLETE_OFS = 54;		//パレット

	//ビットマップコアヘッダ(OS/2)
	static const std::int16_t BCH_HEADERSIZE = 12;
	static const std::int16_t BCH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
	static const std::int16_t BCH_WIDTH_OFS = 18;		//画像の幅[pixel]
	static const std::int16_t BCH_HEIGHT_OFS = 20;		//画像の高さ[pixel]
	static const std::int16_t BCH_PLANENUM_OFS = 22;		//プレーン数
	static const std::int16_t BCH_BITCOUNT_OFS = 24;		//色ビット数[bit]
}


//----------------------------------------------------------
//
// Bitmapデコーダクラス
//
//----------------------------------------------------------

//コンストラクタ
fw::BitmapDecoder::BitmapDecoder() :
	bitmap_()
{
}
fw::BitmapDecoder::BitmapDecoder(const AImage& bitmap) :
	bitmap_(bitmap)
{
}

//RGBAデコード
bool fw::BitmapDecoder::decodeRgba(AImage& rgba, Size& imageSize, std::int8_t bytePerPixel)
{
	//ファイルタイプを取得
	std::uint8_t fileType[2];
	fw::BinReader::read1ByteLe(&this->bitmap_[BFH_FILETYPE_OFS], &fileType[0]);
	fw::BinReader::read1ByteLe(&this->bitmap_[BFH_FILETYPE_OFS + 1], &fileType[1]);
	if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
		//BITMAPデータでない
		return false;
	}

	//ファイル先頭から画像データまでのオフセットを取得
	std::uint32_t imageOffset;
	fw::BinReader::read4ByteLe(&this->bitmap_[BFH_IMAGEOFS_OFS], &imageOffset);

	//情報ヘッダサイズを取得
	std::uint32_t infoHeaderSize;
	fw::BinReader::read4ByteLe(&this->bitmap_[BIH_HEADERSIZE_OFS], &infoHeaderSize);

	if (infoHeaderSize == BIH_HEADERSIZE) {
		//Windowsフォーマット

		//画像の幅と高さを取得
		std::uint32_t width, height;
		fw::BinReader::read4ByteLe(&this->bitmap_[BIH_WIDTH_OFS], &width);
		fw::BinReader::read4ByteLe(&this->bitmap_[BIH_HEIGHT_OFS], &height);

		//色ビット数を取得
		std::uint16_t bitCount;
		fw::BinReader::read2ByteLe(&this->bitmap_[BIH_BITCOUNT_OFS], &bitCount);

		//パレット数を取得
		std::uint32_t palleteNum;
		fw::BinReader::read4ByteLe(&this->bitmap_[BIH_PALLETENUM_OFS], &palleteNum);
		if ((palleteNum == 0) && (bitCount <= 8)) {
			//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
			palleteNum = (1 << bitCount);
		}

		//パレットデータを取得
		AColor pallete(palleteNum);
		std::uint32_t readIndex = BIH_PALLETE_OFS;
		for (std::uint32_t i = 0; i < palleteNum; i++) {
			fw::BinReader::read1ByteLe(&this->bitmap_[readIndex + 0], &pallete[i].b);
			fw::BinReader::read1ByteLe(&this->bitmap_[readIndex + 1], &pallete[i].g);
			fw::BinReader::read1ByteLe(&this->bitmap_[readIndex + 2], &pallete[i].r);
			fw::BinReader::read1ByteLe(&this->bitmap_[readIndex + 3], &pallete[i].a);
			readIndex += 4;
		}

		//RGBA出力データを確保
		std::int32_t imageNum = width * height * 4;
		new(&rgba) AImage(imageNum);

		//RGBA出力データを取得
		for (std::uint32_t h = 0; h < height; h++) {
			std::uint32_t offset = (height - h - 1) * width * 4;
			for (std::uint32_t w = 0; w < width; w++) {
				std::uint8_t index;
				fw::BinReader::read1ByteLe(&this->bitmap_[imageOffset], &index);
				imageOffset++;

				rgba[offset + 0] = pallete[index].r;
				rgba[offset + 1] = pallete[index].g;
				rgba[offset + 2] = pallete[index].b;
				rgba[offset + 3] = 255;
				offset += 4;
			}
		}

		imageSize.w = std::int16_t(width);
		imageSize.h = std::int16_t(height);
		bytePerPixel = 4;
	}
	else if (infoHeaderSize == BCH_HEADERSIZE) {
		//OS/2フォーマット
	}
	else {
		//異常なBITMAPデータ
		return false;
	}

	return true;
}
