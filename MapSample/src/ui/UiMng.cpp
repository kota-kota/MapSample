#include "UiMng.hpp"
#include "image/Image.hpp"


namespace {
	struct ImageFile {
		std::uint32_t	id_;
		fw::ImageFormat	format_;
		std::string		bodyFile_;
		std::string		blendFile_;
	};
	static ImageFile imageFiles[] = {
		//{ 0x11, fw::ImageFormat::PNG, "./data/png/colorType0_depth1.png", "", },
		//{ 0x12, fw::ImageFormat::PNG, "./data/png/colorType0_depth2.png", "", },
		//{ 0x13, fw::ImageFormat::PNG, "./data/png/colorType0_depth4.png", "", },
		//{ 0x14, fw::ImageFormat::PNG, "./data/png/colorType0_depth8.png", "", },
		//{ 0x21, fw::ImageFormat::PNG, "./data/png/colorType2_depth8.png", "", },
		//{ 0x31, fw::ImageFormat::PNG, "./data/png/colorType3_depth1.png", "", },
		//{ 0x32, fw::ImageFormat::PNG, "./data/png/colorType3_depth2.png", "", },
		//{ 0x33, fw::ImageFormat::PNG, "./data/png/colorType3_depth4.png", "", },
		//{ 0x34, fw::ImageFormat::PNG, "./data/png/colorType3_depth8.png", "", },
		//{ 0x81, fw::ImageFormat::BMP, "./data/bitmap/os-1.bmp", "", },
		//{ 0x82, fw::ImageFormat::BMP, "./data/bitmap/os-4.bmp", "", },
		//{ 0x83, fw::ImageFormat::BMP, "./data/bitmap/os-8.bmp", "", },
		//{ 0x84, fw::ImageFormat::BMP, "./data/bitmap/os-24.bmp", "", },
		//{ 0x00, fw::ImageFormat::BMP, "./data/bitmap/dog2.bmp", "", },
		{ 0x91, fw::ImageFormat::BMP, "./data/bitmap/win-1.bmp", "", },
		{ 0x92, fw::ImageFormat::BMP, "./data/bitmap/win-4.bmp", "", },
		{ 0x93, fw::ImageFormat::BMP, "./data/bitmap/win-8.bmp", "", },
		//{ 0x94, fw::ImageFormat::BMP, "./data/bitmap/win-24.bmp", "", },
		//{ 0x95, fw::ImageFormat::BMP, "./data/bitmap/win-32.bmp", "", },
	};
	static const std::int32_t imageFilesNum = sizeof(imageFiles) / sizeof(ImageFile);
}


//----------------------------------------------------------
//
// UI管理クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMng::UiMng(fw::DrawIF* drawIF) :
	drawIF_(drawIF), mapPos_(), screenPos_(), touchPos_(), dragPos_(), isTouchOn_(false), isDragOn_(false),
	texBasePos_(), texImageList_()
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	if (this->drawIF_ != nullptr) {
		//地図中心座標
		this->mapPos_ = { 63230028, 16092608, 0 };

		//画面中心座標
		std::Size screenSize;
		this->drawIF_->getScreenSize(&screenSize);
		this->screenPos_ = { screenSize.width / 2, screenSize.height / 2, 0 };

		//テクスチャ基準座標(画面左上になるように)
		this->texBasePos_ = { this->mapPos_.x - this->screenPos_.x, this->mapPos_.y + this->screenPos_.y, 0 };

		//テクスチャイメージリスト
		this->texImageList_ = new TexImage[imageFilesNum];
	}
}

//デストラクタ
ui::UiMng::~UiMng()
{
	delete[] this->texImageList_;
}

//タッチON
void ui::UiMng::setTouchOn(std::CoordI touchPos)
{
	//タッチ座標を保持
	this->touchPos_ = touchPos;

	//タッチスクロールをONとする
	this->isTouchOn_ = true;
	this->isDragOn_ = false;
}

//タッチOFF
void ui::UiMng::setTouchOff()
{
	if (this->isTouchOn_) {
		//タッチスクロール

		//差分座標を計算
		std::CoordI diffPos;
		diffPos.x = this->screenPos_.x - this->touchPos_.x;
		diffPos.y = this->screenPos_.y - this->touchPos_.y;

		//地図座標に反映
		this->mapPos_.x -= diffPos.x;
		this->mapPos_.y += diffPos.y;
	}
	else {
		//何もしない
	}

	this->isTouchOn_ = false;
	this->isDragOn_ = false;
}

//ドラッグ
void ui::UiMng::setDrag(std::CoordI dragPos)
{
	//ドラッグスクロールをONとする
	this->isTouchOn_ = false;
	this->isDragOn_ = true;

	//差分座標を計算
	std::CoordI diffPos;
	diffPos.x = this->touchPos_.x - dragPos.x;
	diffPos.y = this->touchPos_.y - dragPos.y;

	//地図座標に反映
	this->mapPos_.x += diffPos.x;
	this->mapPos_.y -= diffPos.y;

	this->touchPos_ = dragPos;
}

//描画
void ui::UiMng::draw()
{
	this->drawIF_->makeCurrent(true);
	this->drawIF_->setup(this->mapPos_);
	{
		std::Color defColor = { 230, 231, 232, 255 };
		this->drawIF_->clear(defColor);
	}
	{
		std::CoordI texBasePos = this->texBasePos_;
		for (std::int32_t i = 0; i < imageFilesNum; i++) {
			std::uint32_t* texId = &this->texImageList_[i].texId_;
			fw::Image* image = &this->texImageList_[i].image_;
			if (*texId == 0) {
				//画像ファイルから画像オブジェクト作成
				new(image) fw::Image(imageFiles[i].id_, fw::ImageType::SOFT, imageFiles[i].format_);
				image->Create(imageFiles[i].bodyFile_, 0, 0);

				//デコード
				fw::Image rgba;
				image->DecodeRgba8888(&rgba);

				//テクスチャ作成
				this->drawIF_->createTextures(&rgba, texId);
			}

			std::CoordI ofs = { image->Width(), image->Height(), 0 };

			std::vector<std::CoordTex> coords(4);
			coords[0] = { 0.0, 0.0, texBasePos.x, texBasePos.y, texBasePos.z };
			coords[1] = { 1.0, 0.0, texBasePos.x + ofs.x, texBasePos.y, texBasePos.z };
			coords[2] = { 0.0, 1.0, texBasePos.x, texBasePos.y - ofs.y, texBasePos.z };
			coords[3] = { 1.0, 1.0, texBasePos.x + ofs.x, texBasePos.y - ofs.y, texBasePos.z };

			this->drawIF_->drawTextrue(coords, *texId);

			texBasePos.x += ofs.x;
		}
	}
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(1);
		coords[0] = { 63230028, 16092608, 0 };
		this->drawIF_->drawPoints(coords, colors, 10.0f);
	}
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(5);
		coords[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coords[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coords[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coords[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coords[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->drawIF_->drawLines(coords, colors, 2.0f);
	}

	this->drawIF_->swapBuffers();
	this->drawIF_->makeCurrent(false);
}
