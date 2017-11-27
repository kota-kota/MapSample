#ifndef INCLUDED_LOCALIMAGE_HPP
#define INCLUDED_LOCALIMAGE_HPP

#include "Std.hpp"
#include "image/Image.hpp"
#include <vector>

namespace ui
{
	//ソフト持ち画像ID
	enum EN_LocalImageID : std::int16_t {
		INVALID = 0,

		BMP01,
		BMP02,
		BMP03,
		BMP04,
		BMP05,
		BMP06,
		BMP07,
		BMP08,
		BMP09,
		BMP10,

		PNG01,
		PNG02,
		PNG03,
		PNG04,
		PNG05,
		PNG06,
		PNG07,
		PNG08,
		PNG09,

		JPEG01,
		JPEG02,
	};

	struct LocalImageData {
		EN_LocalImageID		id_;
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
		void getImage(const EN_LocalImageID id, LocalImageData* const localImage);

		//コピーコンストラクタ(禁止)
		LocalImage(const LocalImage& org) = delete;
		//代入演算子(禁止)
		LocalImage& operator=(const LocalImage& org) = delete;
	};
}

#endif //INCLUDED_LOCALIMAGE_HPP
