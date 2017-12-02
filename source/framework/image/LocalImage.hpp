#ifndef INCLUDED_LOCALIMAGE_HPP
#define INCLUDED_LOCALIMAGE_HPP

#include "Std.hpp"
#include "image/Image.hpp"
#include <vector>

namespace fw {

	//ソフト持ち画像ID
	static const std::uint16_t D_IMAGEID_INVALID = 0x0000;
	static const std::uint16_t D_IMAGEID_BMP = 0x0000;
	static const std::uint16_t D_IMAGEID_BMP_01 = D_IMAGEID_BMP | 0x0001;
	static const std::uint16_t D_IMAGEID_BMP_02 = D_IMAGEID_BMP | 0x0002;
	static const std::uint16_t D_IMAGEID_BMP_03 = D_IMAGEID_BMP | 0x0003;
	static const std::uint16_t D_IMAGEID_BMP_04 = D_IMAGEID_BMP | 0x0004;
	static const std::uint16_t D_IMAGEID_BMP_05 = D_IMAGEID_BMP | 0x0005;
	static const std::uint16_t D_IMAGEID_BMP_06 = D_IMAGEID_BMP | 0x0006;
	static const std::uint16_t D_IMAGEID_BMP_07 = D_IMAGEID_BMP | 0x0007;
	static const std::uint16_t D_IMAGEID_BMP_08 = D_IMAGEID_BMP | 0x0008;
	static const std::uint16_t D_IMAGEID_BMP_09 = D_IMAGEID_BMP | 0x0009;
	static const std::uint16_t D_IMAGEID_BMP_0A = D_IMAGEID_BMP | 0x000A;
	static const std::uint16_t D_IMAGEID_PNG = 0x0100;
	static const std::uint16_t D_IMAGEID_PNG_01 = D_IMAGEID_PNG | 0x0001;
	static const std::uint16_t D_IMAGEID_PNG_02 = D_IMAGEID_PNG | 0x0002;
	static const std::uint16_t D_IMAGEID_PNG_03 = D_IMAGEID_PNG | 0x0003;
	static const std::uint16_t D_IMAGEID_PNG_04 = D_IMAGEID_PNG | 0x0004;
	static const std::uint16_t D_IMAGEID_PNG_05 = D_IMAGEID_PNG | 0x0005;
	static const std::uint16_t D_IMAGEID_PNG_06 = D_IMAGEID_PNG | 0x0006;
	static const std::uint16_t D_IMAGEID_PNG_07 = D_IMAGEID_PNG | 0x0007;
	static const std::uint16_t D_IMAGEID_PNG_08 = D_IMAGEID_PNG | 0x0008;
	static const std::uint16_t D_IMAGEID_PNG_09 = D_IMAGEID_PNG | 0x0009;
	static const std::uint16_t D_IMAGEID_JPEG = 0x0200;
	static const std::uint16_t D_IMAGEID_JPEG_01 = D_IMAGEID_JPEG | 0x0001;
	static const std::uint16_t D_IMAGEID_JPEG_02 = D_IMAGEID_JPEG | 0x0002;

	struct LocalImageData {
		std::uint16_t		id_;
		fw::EN_ImageFormat	format_;
		std::uint8_t*		body_;
		std::int32_t		bodySize_;
		std::uint8_t*		blend_;
		std::int32_t		blendSize_;
	};

	//ソフト持ち画像管理クラス
	class LocalImage {
		//メンバ変数
		std::vector<LocalImageData> imageList_;

	public:
		//コンストラクタ
		LocalImage();
		//デストラクタ
		~LocalImage();
		//作成
		void create();
		//解放
		void free();
		//取得
		void getImage(const std::uint16_t id, LocalImageData* const localImage);

		//コピーコンストラクタ(禁止)
		LocalImage(const LocalImage& org) = delete;
		//代入演算子(禁止)
		LocalImage& operator=(const LocalImage& org) = delete;
	};
}

#endif //INCLUDED_LOCALIMAGE_HPP
