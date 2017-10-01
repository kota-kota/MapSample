#ifndef INCLUDED_BITMAP_HPP
#define INCLUDED_BITMAP_HPP

#include "FwStd.hpp"

namespace fw
{
	//----------------------------------------------------------
	//
	// Bitmapデコーダクラス
	//
	//----------------------------------------------------------

	class BitmapDecoder {
		//メンバ変数
		AImage	bitmap_;	//Bitmap画像

	public:
		//コンストラクタ
		BitmapDecoder();
		BitmapDecoder(const AImage& bitmap);
		//RGBAデコード
		bool decodeRgba(AImage& rgba, Size& imageSize, std::int8_t bytePerPixel);
	};
}

#endif //INCLUDED_BITMAP_HPP
