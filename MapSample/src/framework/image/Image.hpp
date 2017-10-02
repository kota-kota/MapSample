#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "Std.hpp"
#include <string>

namespace fw
{
	//----------------------------------------------------------
	//
	// 定数定義
	//
	//----------------------------------------------------------

	//画像タイプ
	enum ImageType {
		UNKNOWN,		//未知
		RGB888,			//RGB888画像
		RGBA8888,		//RGBA8888画像
		BMP,			//BITMAP画像
		PNG,			//PNG画像
	};

	static const std::uint32_t BYTE_PER_PIXEL_RGBA8888 = 4;	//RGBA8888画像の1ピクセルあたりのバイト数
	static const std::uint32_t PALLETE_MAXSIZE = 256 * 4;	//パレット最大サイズ(256色*4バイト)


	//----------------------------------------------------------
	//
	// 画像クラス
	//
	//----------------------------------------------------------

	class Image {
		//メンバ変数
		std::byte_t*	body_;		//本体データ
		size_t			bodySize_;	//本体データサイズ
		std::byte_t*	blend_;		//ブレンドデータ
		size_t			blendSize_;	//ブレンドデータサイズ
		ImageType		type_;		//タイプ
		std::uint32_t	width_;		//幅
		std::uint32_t	height_;	//高さ

	public:
		//コンストラクタ
		Image();
		//デストラクタ
		~Image();
		//本体データ作成
		bool CreateBody(const std::byte_t* const body, const size_t bodySize, const ImageType type, const std::uint32_t width, const std::uint32_t height);
		//ブレンドデータ作成
		bool CreateBlend(const std::byte_t* const blend, const size_t blendSize, const ImageType type);
		//解放
		void Free();
		//本体データ取得
		inline const std::byte_t* const Body() const { return this->body_; }
		//本体データサイズ取得
		inline size_t BodySize() const { return this->bodySize_; }
		//ブレンドデータ取得
		inline const std::byte_t* const Blend() const { return this->blend_; }
		//ブレンドデータサイズ取得
		inline size_t BlendSize() const { return this->blendSize_; }
		//画像タイプ取得
		inline ImageType Type() const { return this->type_; }
		//幅取得
		inline std::uint32_t Width() const { return this->width_; }
		//高さ取得
		inline std::uint32_t Height() const { return this->height_; }

	private:
		//コピーコンストラクタ(禁止)
		Image(const Image& org) = delete;
		//代入演算子(禁止)
		Image& operator=(const Image& org) = delete;
	};


	//----------------------------------------------------------
	//
	// 画像ライブラリクラス
	//
	//----------------------------------------------------------

	class ImageLib {
	public:
		//ファイルから画像オブジェクト作成(body: [true]本体画像 [false]ブレンド画像)
		static bool CreateImage(Image* const outImage, const std::string& file, const ImageType type, const bool body = true);
		//バイナリデータから画像オブジェクト作成(body: [true]本体画像 [false]ブレンド画像)
		static bool CreateImage(Image* const outImage, const std::byte_t* const data, const size_t dataSize, const ImageType type, const bool body = true, const std::uint32_t width = 0, const std::uint32_t height = 0);
		//Bitmap画像からRGBA8888画像へデコード
		static void DecodeRgba8888FromBitmap(Image* const outImage, const Image& inImage);

	private:
		//----------------------------------------------------------
		//
		// Bitmap画像処理ライブラリクラス
		//
		//----------------------------------------------------------
		class Bitmap {
		public:
			//Bitmap判定
			static bool IsBitmap(const std::byte_t* const bmpData);
			//幅高さを取得
			static void GetWH(const std::byte_t* const bmpData, std::uint32_t* const width, std::uint32_t* const height);
			//RGBA8888画像へデコード
			static void DecodeRgba8888(std::byte_t** const outData, const std::byte_t* const bmpData);
		};
	};
}

#endif //INCLUDED_IMAGE_HPP
