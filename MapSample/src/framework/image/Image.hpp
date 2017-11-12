#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "Std.hpp"
#include <string>

namespace fw
{
	//画像タイプ
	enum EN_ImageType : std::uint16_t {
		D_IMAGETYPE_LOCAL,	//ソフト持ち画像
		D_IMAGETYPE_DB,		//DB持ち画像
	};

	//画像フォーマット
	enum EN_ImageFormat : std::uint16_t {
		D_IMAGEFORMAT_RGBA8888,		//RGBA8888画像
		D_IMAGEFORMAT_BMP,			//BITMAP画像
		D_IMAGEFORMAT_PNG,			//PNG画像
		D_IMAGEFORMAT_JPEG,			//JPEG画像
	};

	//画像
	struct Image {
		struct ImageData {
			std::uint8_t*	data_;			//データ
			std::int32_t	dataSize_;		//データサイズ
			std::int32_t	width_;			//幅
			std::int32_t	height_;		//高さ
			std::int32_t	transColor_;	//透過色
			std::uint16_t	isPixelFlip_;	//ピクセル反転有無[0:反転しない 1:反転する]
			std::uint16_t	isChgPallete_;	//パレット差し替え有無[0:差し替えない 1:差し替える]
			std::uint8_t*	pallete_;		//パレットデータ(isChgPallete_==1の場合のみ)
			std::int32_t	palleteNum_;	//パレット数
		};
		std::uint16_t	id_;			//画像ID
		EN_ImageType	type_;			//画像タイプ(ソフト持ち/DB持ち)
		EN_ImageFormat	format_;		//画像フォーマット
		std::uint8_t	isFlip_;		//上下反転有無[0:反転しない 1:反転する]
		std::uint8_t	isBlend_;		//ブレンド有無[0:ブレンドなし 1:ブレンドあり]
		ImageData		body_;			//本体画像データ
		ImageData		blend_;			//ブレンド画像データ(isBlend_==1の場合のみ)
	};

	//画像デコードクラス
	class ImageDecorder {
		std::uint8_t*	decode_;		//デコードデータ
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

	private:
		//初期化
		void init();
		//Bitmap画像デコード
		std::int32_t decodeBitmap(const Image& image);
		//PNG画像デコード
		std::int32_t decodePng(const Image& image);
		//JPEG画像デコード
		std::int32_t decodeJpeg(const Image& image);
	};
}

#endif //INCLUDED_IMAGE_HPP
