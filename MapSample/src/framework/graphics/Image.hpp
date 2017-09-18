#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "FwStd.hpp"

namespace fw
{
	//----------------------------------------------------------
	//
	// 画像タイプ
	//
	//----------------------------------------------------------
	enum EN_ImageType {
		D_IMAGETYPE_UNKNOWN,		//未知
		D_IMAGETYPE_RGB888,			//RGB888画像
		D_IMAGETYPE_RGBA8888,		//RGBA8888画像
		D_IMAGETYPE_BMP,			//BITMAP画像
		D_IMAGETYPE_PNG,			//PNG画像
		D_IMAGETYPE_JPEG,			//JPEG画像
	};


	//----------------------------------------------------------
	//
	// 画像クラス
	//
	//----------------------------------------------------------

	class Image {
		//メンバ変数
		AImage			aImage_;	//画像
		EN_ImageType	imageType_;	//画像タイプ
		std::int16_t	width_;		//幅
		std::int16_t	height_;	//高さ
		std::int32_t	bpp_;		//ピクセルあたりのバイト数

	public:
		//コンストラクタ
		Image();
		Image(const AImage& aImage);
		Image(const AImage& aImage, const EN_ImageType imageType, const std::int16_t w, const std::int16_t h, const std::int32_t bpp);
		//画像(AImage)取得
		AImage* getAImage();
	};


	//----------------------------------------------------------
	//
	// 画像デコーダクラス
	//
	//----------------------------------------------------------

	class ImageDecorder {
		Image*	image_;

	public:
		//コンストラクタ
		ImageDecorder(Image* const image);
		//Bitmapデコーダ
		void decodeRgbaImage(Image* const rgba);
	};
}

#endif //INCLUDED_IMAGE_HPP
