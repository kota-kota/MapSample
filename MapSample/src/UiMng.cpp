#include "ui/UiMng.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include "ui/UiMng.hpp"

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::string;
using std::vector;
using ui::UiMng;
using draw::DrawIF;
using cmn::ColorU8;
using cmn::CoordI16;
using cmn::CoordI32;
using cmn::Area;

//コンストラクタ
UiMng::UiMng(DrawIF* drawIF)
	: mDrawIF(drawIF), mDrawSetup()
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	this->mDrawSetup.mMapPos = { 63230028, 16092608, 0 };
	if (this->mDrawIF != nullptr) {
		Area area;
		this->mDrawIF->getDrawArea(&area);
		this->mDrawSetup.mScreenPos = { area.w / 2, area.h / 2, 0 };
	}

	this->mFile.create("./image/bitmap/win-8.bmp");
	this->mFile.open("rb");
	size_t fileSize = this->mFile.getFileSize();
	vector<uint8_t> buf(fileSize);
	this->mFile.read(0, fileSize, &buf);

	this->mBitmap.create(buf);
	this->mBitmap.decode();
}

//画面中心位置を設定
void UiMng::setScreenPosition(CoordI16 screenPos)
{
	CoordI16 diffPos;
	diffPos.x = screenPos.x - this->mDrawSetup.mScreenPos.x;
	diffPos.y = screenPos.y - this->mDrawSetup.mScreenPos.y;
	this->mDrawSetup.mMapPos.x += diffPos.x;
	this->mDrawSetup.mMapPos.y += diffPos.y;

	this->mDrawSetup.mScreenPos = screenPos;
}

//描画
void UiMng::draw()
{
	this->mDrawIF->makeCurrent(true);
	this->mDrawIF->setup(this->mDrawSetup);
	{
		ColorU8 defColor = { 230, 231, 232, 255 };
		this->mDrawIF->clear(defColor);
	}
	{
		ColorU8 color = { 255, 0, 0, 255 };
		vector<CoordI32> coord(1);
		coord[0] = { 63230028, 16092608, 0 };
		this->mDrawIF->drawPoint(color, coord, 10.0f);
	}
	{
		ColorU8 color = { 255, 0, 0, 255 };
		vector<CoordI32> coord(5);
		coord[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coord[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coord[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coord[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coord[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->mDrawIF->drawLine(color, coord, 2.0f);
	}
	{
		vector<uint8_t> texture;
		int32_t width, height, bytePerPixel;
		this->mBitmap.getRaw(&texture);
		this->mBitmap.getRawInfo(&width, &height, &bytePerPixel);

		Area textureSize = {width, height};
		vector<CoordI32> coord(8);
		coord[0] = { 0, 0, 0 };
		coord[1] = { 63230028 - width, 16092608 + height, 0 };
		coord[2] = { 1, 0, 0 };
		coord[3] = { 63230028 + width, 16092608 + height, 0 };
		coord[4] = { 0, 1, 0 };
		coord[5] = { 63230028 - width, 16092608 - height, 0 };
		coord[6] = { 1, 1, 0 };
		coord[7] = { 63230028 + width, 16092608 - height, 0 };
		this->mDrawIF->drawTextrue(coord, texture, textureSize);
	}

	this->mDrawIF->swapBuffers();
	this->mDrawIF->makeCurrent(false);
}
