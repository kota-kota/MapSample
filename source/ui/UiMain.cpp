#include "UiMain.hpp"
#include "UiMng.hpp"
#include "draw/DrawIF.hpp"


//----------------------------------------------------------
//
// UIメインクラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMain::UiMain(fw::DrawIF* drawIF) :
	drawIF_(drawIF), uiMng_(nullptr)
{
	this->uiMng_ = new UiMng(drawIF);
	this->uiMng_->start();
}

//デストラクタ
ui::UiMain::~UiMain()
{
	this->uiMng_->end();
	if (this->uiMng_ != nullptr) {
		delete this->uiMng_;
	}
}

//タッチON
void ui::UiMain::touchOn(std::CoordI touchPos)
{
	this->uiMng_->setTouchOn(touchPos);
}

//タッチOFF
void ui::UiMain::touchOff()
{
	this->uiMng_->setTouchOff();
}

//タッチ移動
void ui::UiMain::touchMove(std::CoordI dragPos)
{
	this->uiMng_->setDrag(dragPos);
}

//描画
void ui::UiMain::draw()
{
	this->uiMng_->draw();
}
