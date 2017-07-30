#include "UiMng.hpp"

using std::string;
using cmn::Color;
using cmn::CoordI16;
using cmn::CoordI32;
using cmn::Size;
using cmn::CoordVec;
using cmn::Image;
using cmn::Binary;
using graphics::DrawIF;
using ui::UiMng;

//コンストラクタ
UiMng::UiMng(DrawIF* drawIF)
	: drawIF_(drawIF), mapPos_(), screenPos_(), touchPos_(), dragPos_(), isTouchOn_(false), isDragOn_(false)
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	if (this->drawIF_ != nullptr) {
		//地図中心座標
		this->mapPos_ = { 63230028, 16092608, 0 };

		//画面中心座標
		Size drawSize;
		this->drawIF_->getDrawSize(drawSize);
		this->screenPos_ = { drawSize.w / 2, drawSize.h / 2, 0 };

		this->file_.create("./data_image/bitmap/win-8.bmp");
		this->file_.open("rb");
		size_t fileSize = this->file_.getFileSize();
		Binary data(fileSize);
		this->file_.read(0, fileSize, data);

		this->bitmap_.create(data);
		this->bitmap_.decode();
	}
}

//タッチON
void UiMng::setTouchOn(CoordI16 touchPos)
{
	//タッチ座標を保持
	this->touchPos_ = touchPos;

	//タッチスクロールをONとする
	this->isTouchOn_ = true;
	this->isDragOn_ = false;
}

//タッチOFF
void UiMng::setTouchOff()
{
	if (this->isTouchOn_) {
		//タッチスクロール

		//差分座標を計算
		CoordI16 diffPos;
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
void UiMng::setDrag(CoordI16 dragPos)
{
	//ドラッグスクロールをONとする
	this->isTouchOn_ = false;
	this->isDragOn_ = true;

	//差分座標を計算
	CoordI16 diffPos;
	diffPos.x = this->touchPos_.x - dragPos.x;
	diffPos.y = this->touchPos_.y - dragPos.y;

	//地図座標に反映
	this->mapPos_.x += diffPos.x;
	this->mapPos_.y -= diffPos.y;

	this->touchPos_ = dragPos;
}

//描画
void UiMng::draw()
{
	this->drawIF_->makeCurrent(true);
	this->drawIF_->setup(this->mapPos_);
	{
		Color defColor = { 230, 231, 232, 255 };
		this->drawIF_->clear(defColor);
	}
	{
		Image texture;
		int16_t width, height;
		int8_t bytePerPixel;
		this->bitmap_.getRaw(texture, width, height, bytePerPixel);

		Size texSize = { width, height };
		CoordVec coord(8);
		coord[0] = { 0, 0, 0 };
		coord[1] = { 63230028 - width, 16092608 + height, 0 };
		coord[2] = { 1, 0, 0 };
		coord[3] = { 63230028 + width, 16092608 + height, 0 };
		coord[4] = { 0, 1, 0 };
		coord[5] = { 63230028 - width, 16092608 - height, 0 };
		coord[6] = { 1, 1, 0 };
		coord[7] = { 63230028 + width, 16092608 - height, 0 };
		this->drawIF_->drawTextrue(coord, texture, texSize);
	}
	{
		Color color = { 255, 0, 0, 255 };
		CoordVec coord(1);
		coord[0] = { 63230028, 16092608, 0 };
		this->drawIF_->drawPoint(color, coord, 10.0f);
	}
	{
		Color color = { 255, 0, 0, 255 };
		CoordVec coord(5);
		coord[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coord[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coord[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coord[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coord[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->drawIF_->drawLine(color, coord, 2.0f);
	}

	this->drawIF_->swapBuffers();
	this->drawIF_->makeCurrent(false);
}
