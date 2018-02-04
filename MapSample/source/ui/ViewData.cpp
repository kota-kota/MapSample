#include "ViewData.hpp"
#include "draw/DrawIF.hpp"



//----------------------------------------------------------
//
// 表示物(点)クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewPoint::ViewPoint(const fw::DrawCoords& coords, const fw::DrawColors& colors, const std::float_t width) :
	coords_(coords), colors_(colors), width_(width)
{
}

//描画
void ui::ViewPoint::draw(fw::DrawIF* const drawIF)
{
	drawIF->drawPoints(this->coords_, this->colors_, this->width_);
}


//----------------------------------------------------------
//
// 表示物(線)クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewLine::ViewLine(const fw::DrawCoords& coords, const fw::DrawColors& colors, const std::float_t width) :
	coords_(coords), colors_(colors), width_(width)
{
}

//描画
void ui::ViewLine::draw(fw::DrawIF* const drawIF)
{
	drawIF->drawLines(this->coords_, this->colors_, this->width_);
}


//----------------------------------------------------------
//
// 表示物(ポリゴン)クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewPolygon::ViewPolygon(const fw::DrawCoords& coords, const fw::DrawColors& colors) :
	coords_(coords), colors_(colors)
{
}

//描画
void ui::ViewPolygon::draw(fw::DrawIF* const drawIF)
{
	drawIF->drawPolygons(this->coords_, this->colors_);
}



//----------------------------------------------------------
//
// 表示物(イメージ)クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewImage::ViewImage(const std::CoordI& coord, const fw::Image& image) :
	coord_(coord), image_(image)
{
}

//描画
void ui::ViewImage::draw(fw::DrawIF* const drawIF)
{
	drawIF->drawImage(coord_, image_);
}


//----------------------------------------------------------
//
// 表示物(文字列)クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewString::ViewString(const std::CoordI& coord, const std::wstring& str) :
	coord_(coord), str_(str)
{
}

//描画
void ui::ViewString::draw(fw::DrawIF* const drawIF)
{
	drawIF->drawString(coord_, str_.c_str());
}



//----------------------------------------------------------
//
// 表示データクラス
//
//----------------------------------------------------------

//コンストラクタ
ui::ViewData::ViewData() :
	backColor_(), partsList_()
{
}

//デストラクタ
ui::ViewData::~ViewData()
{
}

//背景色設定
void ui::ViewData::setBackColor(const std::ColorUB& backColor)
{
	this->backColor_ = backColor;
}

//描画物設定
void ui::ViewData::setDrawParts(ViewParts* const parts)
{
	this->partsList_.push_back(parts);
}

//描画
void ui::ViewData::draw(fw::DrawIF* const drawIF)
{
	drawIF->clear(this->backColor_);

	for (auto itr = this->partsList_.begin(); itr != this->partsList_.end(); itr++) {
		(*itr)->draw(drawIF);
	}
}
