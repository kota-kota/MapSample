#ifndef INCLUDED_IMAGEBMP_HPP
#define INCLUDED_IMAGEBMP_HPP

#include "Type.hpp"

namespace draw {

	/**
	 * 画像BMPクラス
	 */
	class ImageBMP {

		//パレットカラー構造体
		struct PalColor {
			std::uint8_t	r;
			std::uint8_t	g;
			std::uint8_t	b;
		};

		//BMPフォーマット
		enum EN_BmpFormat {
			INVALID,	//無効
			WINDOWS,	//Windowsフォーマット
			OS2,		//OS/2フォーマット
		};

		//メンバ変数
		std::uint8_t*	bmp_;			//BMPデータ
		std::int32_t	bmpSize_;		//BMPデータサイズ
		EN_BmpFormat	format_;		//BMPタイプ
		std::int32_t	fileSize_;		//ファイルサイズ
		std::int32_t	imageOffset_;	//画像データまでのオフセット
		std::int32_t	width_;			//幅
		std::int32_t	height_;		//高さ
		std::int32_t	bitCount_;		//ビット数
		std::int32_t	compression_;	//圧縮形式
		std::int32_t	imageSize_;		//画像データサイズ
		std::int32_t	palleteNum_;	//パレット数
		std::int32_t	palleteByte_;	//1パレットあたりのバイト数
		std::int32_t	palleteOffset_;	//パレットまでのオフセット

	public:
		//コンストラクタ
		ImageBMP();
		//デストラクタ
		~ImageBMP();
		//作成
		std::int32_t create(std::uint8_t* const bmp, const std::int32_t bmpSize);
		//幅高さ取得
		void getWH(std::int32_t* const width, std::int32_t* const height);
		//RGBA8888画像へデコード
		std::int32_t decode_RGBA8888(std::uint8_t** const decData);

	private:
		//Bitmap情報ヘッダ(Windows)読み込み
		std::int32_t readInfoHeader_WINDOWS();
		//Bitmap情報ヘッダ(OS/2)読み込み
		std::int32_t readInfoHeader_OS2();
		//パディングバイト数を取得
		std::uint32_t getPaddingByte();
		//パレットデータを取得
		std::int32_t getPalleteData(PalColor* const pallete, const std::int32_t numMaxPal);
		//パレットBMP画像からRGBA8888画像へデコード
		std::int32_t decodeRgba8888FromPalleteBitmap(std::uint8_t** const decData);
		//トゥルーカラーBitmap画像からRGBA8888画像へデコード
		std::int32_t decodeRgba8888FromTrueColorBitmap(std::uint8_t** const decData);

		//コピーコンストラクタ(禁止)
		ImageBMP(const ImageBMP& org) = delete;
		//代入演算子(禁止)
		ImageBMP& operator=(const ImageBMP& org) = delete;
	};
}

#endif //INCLUDED_IMAGEBMP_HPP
