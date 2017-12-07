#include "LocalImage.hpp"
#include "image/Image.hpp"
#include "io/File.hpp"


namespace {

	using fw::EN_ImageFormat;

	//画像ファイル構造体
	struct ImageFile {
		std::uint16_t		id_;
		EN_ImageFormat		format_;
		std::string			bodyFile_;
		std::string			blendFile_;
	};

	//画像ファイルテーブル
	static const std::vector<ImageFile> tblImageFiles = {
		{ fw::D_IMAGEID_BMP_01, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/os-1.bmp", "", },
		{ fw::D_IMAGEID_BMP_02, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/os-4.bmp", "", },
		{ fw::D_IMAGEID_BMP_03, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/os-8.bmp", "", },
		{ fw::D_IMAGEID_BMP_04, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/os-24.bmp", "", },
		{ fw::D_IMAGEID_BMP_05, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/dog2.bmp", "", },
		{ fw::D_IMAGEID_BMP_06, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/win-1.bmp", "", },
		{ fw::D_IMAGEID_BMP_07, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/win-4.bmp", "", },
		{ fw::D_IMAGEID_BMP_08, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/win-8.bmp", "", },
		{ fw::D_IMAGEID_BMP_09, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/win-24.bmp", "", },
		{ fw::D_IMAGEID_BMP_0A, EN_ImageFormat::D_IMAGEFORMAT_BMP, "bitmap/win-32.bmp", "", },
		{ fw::D_IMAGEID_PNG_01, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType0_depth1.png", "", },
		{ fw::D_IMAGEID_PNG_02, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType0_depth2.png", "", },
		{ fw::D_IMAGEID_PNG_03, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType0_depth4.png", "", },
		{ fw::D_IMAGEID_PNG_04, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType0_depth8.png", "", },
		{ fw::D_IMAGEID_PNG_05, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType2_depth8.png", "", },
		{ fw::D_IMAGEID_PNG_06, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType3_depth1.png", "", },
		{ fw::D_IMAGEID_PNG_07, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType3_depth2.png", "", },
		{ fw::D_IMAGEID_PNG_08, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType3_depth4.png", "", },
		{ fw::D_IMAGEID_PNG_09, EN_ImageFormat::D_IMAGEFORMAT_PNG, "png/colorType3_depth8.png", "", },
		{ fw::D_IMAGEID_JPEG_01, EN_ImageFormat::D_IMAGEFORMAT_JPEG, "jpeg/testorig.jpg", "", },
		{ fw::D_IMAGEID_JPEG_02, EN_ImageFormat::D_IMAGEFORMAT_JPEG, "jpeg/testimgint.jpg", "", },
	};
}



//----------------------------------------------------------
//
// ソフト持ち画像管理クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::LocalImage::LocalImage() :
	imageList_()
{
}

//デストラクタ
fw::LocalImage::~LocalImage()
{
	//解放
	this->free();
}

//作成
void fw::LocalImage::create()
{
	//dataフォルダパス
	const std::string dataPath = std::D_DATA_PATH;

	if (this->imageList_.empty()) {
		//未作成の場合のみ

		//画像データリストの領域を確保しておく(画像ファイル数分)
		this->imageList_.resize(tblImageFiles.size());

		//画像データリストのはじめのイテレータ
		auto itrImage = this->imageList_.begin();

		//画像ファイル数分ループ
		for (auto itrFile = tblImageFiles.cbegin(); itrFile != tblImageFiles.cend(); itrFile++) {
			std::uint8_t* body = nullptr;
			size_t bodySize = 0;
			std::uint8_t* blend = nullptr;
			size_t blendSize = 0;

			if (!itrFile->bodyFile_.empty()) {
				//本体画像ファイル読み込み
				fw::File bodyFile;
				bodyFile.create(dataPath + "/" + itrFile->bodyFile_);
				bodyFile.open("rb");
				bodySize = bodyFile.getFileSize();
				body = new std::uint8_t[bodySize];
				(void)bodyFile.read(&body, 0, bodySize);
			}

			if (!itrFile->blendFile_.empty()) {
				//ブレンド画像ファイル読み込み
				fw::File blendFile;
				blendFile.create(dataPath + "/" + itrFile->blendFile_);
				blendFile.open("rb");
				blendSize = blendFile.getFileSize();
				blend = new std::uint8_t[blendSize];
				(void)blendFile.read(&blend, 0, blendSize);
			}

			//画像データ作成
			LocalImageData imageData;
			imageData.id_ = itrFile->id_;
			imageData.format_ = itrFile->format_;
			imageData.body_ = body;
			imageData.bodySize_ = bodySize;
			imageData.blend_ = blend;
			imageData.blendSize_ = blendSize;

			//画像データリストに追加
			*itrImage = imageData;
			itrImage++;
		}
	}
	else {
		//画像データリスト作成済みのため何もしない
	}
}

//解放
void fw::LocalImage::free()
{
	//画像データ数分ループ
	for (auto itrImage = this->imageList_.cbegin(); itrImage != this->imageList_.cend(); itrImage++) {
		if (itrImage->body_ != nullptr) {
			delete[] itrImage->body_;
		}
		if (itrImage->blend_ != nullptr) {
			delete[] itrImage->blend_;
		}
	}

	//画像データリストのクリア
	this->imageList_.clear();
}

//取得
void fw::LocalImage::getImage(const std::uint16_t id, LocalImageData* const localImage)
{
	//出力を初期化
	localImage->id_ = D_IMAGEID_INVALID;
	localImage->format_ = EN_ImageFormat::D_IMAGEFORMAT_RGBA8888;
	localImage->body_ = nullptr;
	localImage->bodySize_ = 0;
	localImage->blend_ = nullptr;
	localImage->blendSize_ = 0;

	//画像データ数分ループ
	for (auto itrImage = this->imageList_.cbegin(); itrImage != this->imageList_.cend(); itrImage++) {
		//画像IDが一致するものを検索
		if (itrImage->id_ == id) {
			//画像ID一致
			*localImage = *itrImage;
			break;
		}
	}
}
