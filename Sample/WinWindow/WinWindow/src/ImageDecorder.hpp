#ifndef INCLUDED_IMAGEDECORDER_HPP
#define INCLUDED_IMAGEDECORDER_HPP

#include "Type.hpp"

namespace fw {

	//画像フォーマット
	enum EN_ImageFormat {
		BMP,		//BITMAP画像
		PNG,		//PNG画像
		JPEG,		//JPEG画像
	};

	/**
	 * 画像デコードクラス
	 */
	class ImageDecorder {
		//メンバ変数
		std::uint8_t*	decData_;		//デコードデータ
		std::int32_t	decDataSize_;	//デコードデータサイズ
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ

	public:
		//コンストラクタ
		ImageDecorder();
		//デストラクタ
		~ImageDecorder();
		//RGBA8888画像へデコード
		std::int32_t decode_RGBA8888(const std::char8_t* const bodyFilePath, const std::char8_t* const blendFilePath, const EN_ImageFormat format, const bool isFlip = false);
		std::int32_t decode_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize, std::uint8_t* const blendData, const std::int32_t blendDataSize, const EN_ImageFormat format, const bool isFlip = false);
		//デコードデータ取得
		std::uint8_t* getDecodeData(std::int32_t* const decDataSize, std::int32_t* const width, std::int32_t* const height);

	private:
		//本体BMP画像をRGBA8888画像へデコード
		std::int32_t decodeBMP_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize);
		//ブレンドBMP画像をRGBA8888画像へデコードし、本体デコード画像へブレンド
		std::int32_t blendBMP_RGBA8888(std::uint8_t* const blendData, const std::int32_t blendDataSize);
		//本体PNG画像をRGBA8888画像へデコード
		std::int32_t decodePNG_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize);
		//ブレンドPNG画像をRGBA8888画像へデコードし、本体デコード画像へブレンド
		std::int32_t blendPNG_RGBA8888(std::uint8_t* const blendData, const std::int32_t blendDataSize);
		//RGBA8888画像のブレンド処理
		void blend_RGBA8888(std::uint8_t* const decData_blend);

		//コピーコンストラクタ(禁止)
		ImageDecorder(const ImageDecorder& org) = delete;
		//代入演算子(禁止)
		ImageDecorder& operator=(const ImageDecorder& org) = delete;
	};
}

#endif //INCLUDED_IMAGEDECORDER_HPP
