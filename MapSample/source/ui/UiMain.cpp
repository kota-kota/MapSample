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
	uiMng_(nullptr)
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

//ボタンイベント処理
void ui::UiMain::procButtonEvent(const ui::EN_ButtonEvent buttonEvent, const std::Position& buttonPos)
{
	this->uiMng_->procButtonEvent(buttonEvent, buttonPos);
}
