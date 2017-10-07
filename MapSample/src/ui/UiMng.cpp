#include "UiMng.hpp"
#include "image/Image.hpp"


static const std::int32_t IMAGE_FIES_NUM = 10;
static std::string imageFiles[IMAGE_FIES_NUM] = {
	"./data/bitmap/os-1.bmp",
	"./data/bitmap/os-4.bmp",
	"./data/bitmap/os-8.bmp",
	"./data/bitmap/os-24.bmp",
	"./data/bitmap/dog2.bmp",
	"./data/bitmap/win-1.bmp",
	"./data/bitmap/win-4.bmp",
	"./data/bitmap/win-8.bmp",
	"./data/bitmap/win-24.bmp",
	"./data/bitmap/win-32.bmp",
};


//----------------------------------------------------------
//
// UI管理クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMng::UiMng(fw::DrawIF* drawIF)
	: drawIF_(drawIF), mapPos_(), screenPos_(), touchPos_(), dragPos_(), isTouchOn_(false), isDragOn_(false), texId_()
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	if (this->drawIF_ != nullptr) {
		//地図中心座標
		this->mapPos_ = { 63230028, 16092608, 0 };

		//画面中心座標
		std::Size screenSize;
		this->drawIF_->getScreenSize(&screenSize);
		this->screenPos_ = { screenSize.width / 2, screenSize.height / 2, 0 };

		//テクスチャ
		this->texId_.resize(IMAGE_FIES_NUM, 0);
	}
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
		std::int32_t xBase = 63229700;
		std::int32_t yBase = 16092750;
		std::vector<std::CoordTex> coordsBase(4);
		coordsBase[0] = { 0.0, 0.0, xBase - 32, yBase + 32, 0 };
		coordsBase[1] = { 1.0, 0.0, xBase + 32, yBase + 32, 0 };
		coordsBase[2] = { 0.0, 1.0, xBase - 32, yBase - 32, 0 };
		coordsBase[3] = { 1.0, 1.0, xBase + 32, yBase - 32, 0 };
		for (std::int32_t i = 0; i < this->texId_.size(); i++) {
			if (this->texId_[i] == 0) {
				//BMPファイルから画像オブジェクト作成
				fw::Image bmpImage;
				fw::ImageLib::CreateImage(&bmpImage, imageFiles[i], fw::ImageType::BMP);

				//デコード
				fw::Image rgba;
				fw::ImageLib::DecodeRgba8888FromBitmap(&rgba, bmpImage);

				//テクスチャ作成
				this->drawIF_->createTextures(&rgba, &this->texId_[i]);
			}

			std::vector<std::CoordTex> coords = coordsBase;
			std::int32_t ofs = i * 64;
			coords[0].x += ofs;
			coords[1].x += ofs;
			coords[2].x += ofs;
			coords[3].x += ofs;
			this->drawIF_->drawTextrue(coords, this->texId_[i]);
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
