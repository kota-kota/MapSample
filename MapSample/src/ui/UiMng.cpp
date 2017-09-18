#include "UiMng.hpp"
#include "graphics/Image.hpp"


//----------------------------------------------------------
//
// UI管理クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMng::UiMng(fw::DrawIF* drawIF)
	: drawIF_(drawIF), mapPos_(), screenPos_(), touchPos_(), dragPos_(), isTouchOn_(false), isDragOn_(false), texId_(0)
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	if (this->drawIF_ != nullptr) {
		//地図中心座標
		this->mapPos_ = { 63230028, 16092608, 0 };

		//画面中心座標
		fw::Size drawSize;
		this->drawIF_->getDrawSize(drawSize);
		this->screenPos_ = { drawSize.w / 2, drawSize.h / 2, 0 };
	}
}

//タッチON
void ui::UiMng::setTouchOn(fw::CoordI16 touchPos)
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
		fw::CoordI16 diffPos;
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
void ui::UiMng::setDrag(fw::CoordI16 dragPos)
{
	//ドラッグスクロールをONとする
	this->isTouchOn_ = false;
	this->isDragOn_ = true;

	//差分座標を計算
	fw::CoordI16 diffPos;
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
		fw::Color defColor = { 230, 231, 232, 255 };
		this->drawIF_->clear(defColor);
	}
	{
		if (texId_ == 0) {
			this->file_.create("./data_image/bitmap/win-8.bmp");
			this->file_.open("rb");
			size_t fileSize = this->file_.getFileSize();
			std::int32_t num = std::int32_t(fileSize);
			fw::ABinary data(num);
			this->file_.read(0, fileSize, data);

			fw::AImage aImage = data;
			fw::Image image(aImage);
			fw::ImageDecorder imageDecorder(&image);

			fw::Image rgba2;
			imageDecorder.decodeRgbaImage(&rgba2);

			fw::AImage rgba;
			fw::Size imageSize;
			std::int8_t bytePerPixel = 0;
			fw::BitmapDecoder(data).decodeRgba(rgba, imageSize, bytePerPixel);

			//テクスチャ作成
			this->drawIF_->createTexture(rgba, imageSize, texId_);
		}
		fw::ACoordI32 coords(8);
		coords[0] = { 0, 0, 0 };
		coords[1] = { 63230028 - 16, 16092608 + 16, 0 };
		coords[2] = { 1, 0, 0 };
		coords[3] = { 63230028 + 16, 16092608 + 16, 0 };
		coords[4] = { 0, 1, 0 };
		coords[5] = { 63230028 - 16, 16092608 - 16, 0 };
		coords[6] = { 1, 1, 0 };
		coords[7] = { 63230028 + 16, 16092608 - 16, 0 };
		this->drawIF_->drawTextrue(coords, texId_);
	}
	{
		fw::AColor colors(1);
		colors[0] = { 255, 0, 0, 255 };
		fw::ACoordI32 coords(1);
		coords[0] = { 63230028, 16092608, 0 };
		this->drawIF_->drawPoint(coords, colors, 10.0f);
	}
	{
		fw::AColor colors(1);
		colors[0] = { 255, 0, 0, 255 };
		fw::ACoordI32 coords(5);
		coords[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coords[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coords[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coords[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coords[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->drawIF_->drawLine(coords, colors, 2.0f);
	}

	this->drawIF_->swapBuffers();
	this->drawIF_->makeCurrent(false);
}
