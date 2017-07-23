#include "UiMng.hpp"

using std::string;
using std::vector;
using cmn::Color;
using cmn::CoordI16;
using cmn::CoordI32;
using cmn::Size;
using graphics::DrawIF;
using ui::UiMng;

//コンストラクタ
UiMng::UiMng(DrawIF* drawIF)
	: drawIF_(drawIF), drawSetup_()
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	this->drawSetup_.mapPos_ = { 63230028, 16092608, 0 };
	if (this->drawIF_ != nullptr) {
		Size drawSize;
		this->drawIF_->getDrawSize(drawSize);
		this->drawSetup_.screenPos_ = { drawSize.w / 2, drawSize.h / 2, 0 };
	}

	this->file_.create("./data_image/bitmap/win-8.bmp");
	this->file_.open("rb");
	size_t fileSize = this->file_.getFileSize();
	vector<uint8_t> buf(fileSize);
	this->file_.read(0, fileSize, &buf);

	this->bitmap_.create(buf);
	this->bitmap_.decode();
}

//画面中心位置を設定
void UiMng::setScreenPosition(CoordI16 screenPos)
{
	CoordI16 diffPos;
	diffPos.x = screenPos.x - this->drawSetup_.screenPos_.x;
	diffPos.y = screenPos.y - this->drawSetup_.screenPos_.y;
	this->drawSetup_.mapPos_.x += diffPos.x;
	this->drawSetup_.mapPos_.y += diffPos.y;

	this->drawSetup_.screenPos_ = screenPos;
}

//描画
void UiMng::draw()
{
	this->drawIF_->makeCurrent(true);
	this->drawIF_->setup(this->drawSetup_);
	{
		Color defColor = { 230, 231, 232, 255 };
		this->drawIF_->clear(defColor);
	}
	{
		Color color = { 255, 0, 0, 255 };
		vector<CoordI32> coord(1);
		coord[0] = { 63230028, 16092608, 0 };
		this->drawIF_->drawPoint(color, coord, 10.0f);
	}
	{
		Color color = { 255, 0, 0, 255 };
		vector<CoordI32> coord(5);
		coord[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coord[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coord[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coord[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coord[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->drawIF_->drawLine(color, coord, 2.0f);
	}
	{
		vector<uint8_t> texture;
		int16_t width, height;
		int8_t bytePerPixel;
		this->bitmap_.getRaw(texture);
		this->bitmap_.getRawInfo(width, height, bytePerPixel);

		Size texSize = {width, height};
		vector<CoordI32> coord(8);
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

	this->drawIF_->swapBuffers();
	this->drawIF_->makeCurrent(false);
}
