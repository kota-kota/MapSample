#ifndef INCLUDED_IMAGEPNG_HPP
#define INCLUDED_IMAGEPNG_HPP

#include "Type.hpp"
#include <png.h>
#include <pngstruct.h>

namespace fw {

	/**
	 * 画像PNGクラス
	 */
	class ImagePNG {

		//メンバ変数
		std::uint8_t*	png_;			//PNGデータ
		std::int32_t	pngSize_;		//PNGデータサイズ
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ
		std::int32_t	rowByte_;		//行バイト数
		std::uint8_t	bitDepth_;		//ビット深度
		std::uint8_t	colorType_;		//カラータイプ
		std::int16_t	dmy_;

		png_structp		pngStr_;		//PNG構造ポインタ(解放必要)
		png_infop		pngInfo_;		//PNG情報ポインタ(解放必要)

	public:
		//コンストラクタ
		ImagePNG();
		//デストラクタ
		~ImagePNG();
		//作成
		std::int32_t create(std::uint8_t* const png, const std::int32_t pngSize);
		//幅高さ取得
		void getWH(std::int32_t* const width, std::int32_t* const height);
		//RGBA8888画像へデコード
		std::int32_t decode_RGBA8888(std::uint8_t** const decData);

	private:
		//グレーPNG画像からRGBA8888画像へデコード
		std::int32_t decodeGrayScalePng_RGBA8888(std::uint8_t** const decData, const png_bytepp png);
		//トゥルーカラーPNG画像からRGBA8888画像へデコード
		std::int32_t decodeTrueColorPng_RGBA8888(std::uint8_t** const decData, const png_bytepp png);
		//パレットPNG画像からRGBA8888画像へデコード
		std::int32_t decodePalletePng_RGBA8888(std::uint8_t** const decData, const png_bytepp png);

		//コピーコンストラクタ(禁止)
		ImagePNG(const ImagePNG& org) = delete;
		//代入演算子(禁止)
		ImagePNG& operator=(const ImagePNG& org) = delete;
	};
}

#endif //INCLUDED_IMAGEPNG_HPP
