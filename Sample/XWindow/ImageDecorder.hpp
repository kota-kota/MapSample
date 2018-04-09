#ifndef INCLUDED_IMAGEDECORDER_HPP
#define INCLUDED_IMAGEDECORDER_HPP

#include "Type.hpp"

namespace draw {

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
		std::int32_t decode_RGBA8888(const std::char8_t* const filePath, const EN_ImageFormat format, const bool isFlip = false);
		std::int32_t decode_RGBA8888(std::uint8_t* const data, const std::int32_t dataSize, const EN_ImageFormat format, const bool isFlip = false);
		//デコードデータ取得
		std::uint8_t* getDecodeData(std::int32_t* const decDataSize, std::int32_t* const width, std::int32_t* const height);

	private:
		//コピーコンストラクタ(禁止)
		ImageDecorder(const ImageDecorder& org) = delete;
		//代入演算子(禁止)
		ImageDecorder& operator=(const ImageDecorder& org) = delete;
	};
}

#endif //INCLUDED_IMAGEDECORDER_HPP
