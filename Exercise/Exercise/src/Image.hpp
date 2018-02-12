#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "Type.hpp"
#include <string>

namespace ui {

	//画像フォーマット
	enum EN_ImageFormat : std::uint16_t {
		RGBA8888,		//RGBA8888画像
		BMP,			//BITMAP画像
		PNG,			//PNG画像
	};

	//画像
	struct Image {
		struct ImageData {
			std::uint8_t*	data_;			//データ
			std::int32_t	dataSize_;		//データサイズ
			std::int32_t	width_;			//幅
			std::int32_t	height_;		//高さ
		};
		std::uint16_t	id_;			//画像ID
		EN_ImageFormat	format_;		//画像フォーマット
		std::uint8_t	isBlend_;		//ブレンド有無[0:ブレンドなし 1:ブレンドあり]
		ImageData		body_;			//本体画像データ
		ImageData		blend_;			//ブレンド画像データ(isBlend_==1の場合のみ)
	};

	//画像デコードクラス
	class ImageDecorder {
		std::uint8_t*	decodeData_;	//デコードデータ
		std::int32_t	decodeSize_;	//デコードデータサイズ
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ

	public:
		//コンストラクタ
		ImageDecorder();
		//コピーコンストラクタ(禁止)
		ImageDecorder(const ImageDecorder& org) = delete;
		//代入演算子(禁止)
		ImageDecorder& operator=(const ImageDecorder& org) = delete;
		//デストラクタ
		~ImageDecorder();

		//デコード
		std::int32_t decode(const Image& image);
		//デコードデータ取得
		std::uint8_t* const getDecodeData(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height);
	};
}

#endif //INCLUDED_IMAGE_HPP
