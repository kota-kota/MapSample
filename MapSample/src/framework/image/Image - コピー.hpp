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
	namespace NImageType {
		enum EType {
			UNKNOWN,		//未知
			SOFT,			//ソフト持ち画像
			MAPDB,			//地図DB持ち画像
		};
	}
	using ImageType = NImageType::EType;

	//画像フォーマット
	namespace NImageFormat {
		enum EFormat {
			UNKNOWN,		//未知
			RGB888,			//RGB888画像
			RGBA8888,		//RGBA8888画像
			BMP,			//BITMAP画像
			PNG,			//PNG画像
			JPEG,			//JPEG画像
		};
	}
	using ImageFormat = NImageFormat::EFormat;


	//----------------------------------------------------------
	//
	// 画像クラス
	//
	//----------------------------------------------------------

	class Image {
		//メンバ変数
		std::uint32_t	id_;			//画像ID
		ImageType		type_;			//画像タイプ
		ImageFormat		format_;		//画像フォーマット
		std::uint32_t	width_;			//幅
		std::uint32_t	height_;		//高さ
		std::Color		transColor_;	//透過色
		size_t			bodySize_;		//本体画像データサイズ
		std::byte_t*	body_;			//本体画像データ
		size_t			blendSize_;		//ブレンド画像データサイズ
		std::byte_t*	blend_;			//ブレンド画像データ
		std::uint32_t	extPalleteNum_;	//拡張パレット数
		std::Color*		extPallete_;	//拡張パレット

	public:
		//コンストラクタ
		Image();
		Image(const std::uint32_t id, const ImageType type, const ImageFormat format);
		//デストラクタ
		~Image();
		//本体画像作成(バイナリデータから)
		bool Create(const std::byte_t* const body, const size_t bodySize, const std::uint32_t width, const std::uint32_t height);
		//本体画像作成(ファイルから)
		bool Create(const std::string bodyFile, const std::uint32_t width, const std::uint32_t height);
		//解放
		void Free();
		//ブレンド画像設定(バイナリデータから)
		bool SetBlend(const std::byte_t* const blend, const size_t blendSize);
		//ブレンド画像設定(ファイルから)
		bool SetBlend(const std::string blendFile);
		//透過色設定
		bool SetTransColor(const std::Color transColor);
		//拡張パレット設定
		bool SetExtPallete(const std::Color* const pallete, const std::uint32_t palleteNum);
		//デコード(RGBA8888)
		void DecodeRgba8888(Image* const outImage);

		//画像ID取得
		inline std::uint32_t Id() const { return this->id_; }
		//画像タイプ取得
		inline ImageType Type() const { return this->type_; }
		//幅取得
		inline std::uint32_t Width() const { return this->width_; }
		//高さ取得
		inline std::uint32_t Height() const { return this->height_; }
		//本体画像取得
		inline const std::byte_t* const Body() const { return this->body_; }

		//コピーコンストラクタ(禁止)
		Image(const Image& org) = delete;
		//代入演算子(禁止)
		Image& operator=(const Image& org) = delete;

	private:
		//初期化済み判定
		bool IsInit();
		//作成済み判定
		bool IsCreate();
		//幅高さを取得
		void GetWH(std::uint32_t* const width, std::uint32_t* const height);
	};
}

#endif //INCLUDED_IMAGE_HPP
