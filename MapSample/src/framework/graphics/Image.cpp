#include "Image.hpp"


//----------------------------------------------------------
//
// Bitmap画像処理メソッド群
//
//----------------------------------------------------------

namespace bitmap
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

	//Bitmap画像判定
	static bool isBitmap(fw::AImage& aImage)
	{
		bool isBitmap = true;

		//ファイルタイプを取得
		std::uint8_t fileType[2];
		fw::BinReader::read1ByteLe(aImage + BFH_FILETYPE_OFS, &fileType[0]);
		fw::BinReader::read1ByteLe(aImage + BFH_FILETYPE_OFS + 1, &fileType[1]);
		if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
			//BITMAPデータでない
			isBitmap = false;
		}

		return isBitmap;
	}
}



//----------------------------------------------------------
//
// 画像クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::Image::Image() :
	aImage_(), imageType_(D_IMAGETYPE_UNKNOWN), width_(0), height_(0), bpp_(0)
{
}
fw::Image::Image(const AImage& aImage) :
	aImage_(aImage), imageType_(D_IMAGETYPE_UNKNOWN), width_(0), height_(0), bpp_(0)
{
}
fw::Image::Image(const AImage& aImage, const EN_ImageType imageType, const std::int16_t w, const std::int16_t h, const std::int32_t bpp) :
	aImage_(aImage), imageType_(imageType), width_(w), height_(h), bpp_(bpp)
{
}

//画像(AImage)取得
fw::AImage* fw::Image::getAImage()
{
	return &aImage_;
}



//----------------------------------------------------------
//
// 画像デコーダクラス
//
//----------------------------------------------------------

//コンストラクタ
fw::ImageDecorder::ImageDecorder(Image* const image) :
	image_(image)
{
}

//Bitmapデコーダ
void fw::ImageDecorder::decodeRgbaImage(Image* const rgba)
{
	AImage* aImage = image_->getAImage();
	if (bitmap::isBitmap(*aImage)) {
	}
}
