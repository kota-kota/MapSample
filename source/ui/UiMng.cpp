#include "UiMng.hpp"
#include "UiScreen.hpp"
#include "image/LocalImage.hpp"



namespace {

	//メインタスク
	static void main_uimng_task(ui::UiMng* const uiMng)
	{
		while (uiMng->isStart()) {
		}
	}

	//描画タスク
	static void main_draw_task(ui::UiMng* const uiMng, ui::UiScreen* const screen)
	{
		std::int32_t drawCnt = 0;
		while (uiMng->isStart()) {
			if (screen->isUpdateDraw()) {
				//描画更新必要の場合のみ描画
				drawCnt++;
				printf("<%d>描画必要\n", drawCnt);
				screen->draw();
			}
		}
	}
}


//----------------------------------------------------------
//
// UI管理クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMng::UiMng(fw::DrawIF* drawIF) :
	isStart_(std::EN_OffOn::OFF), mainThread_(), drawThread_(), drawIF_(drawIF), localImage_(nullptr), screen_(nullptr)
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);

	//ソフト持ち画像作成
	this->localImage_ = new fw::LocalImage();
	this->localImage_->create();

	//画面作成
	this->screen_ = new UiScreen(this->drawIF_, this->localImage_, std::D_MAP_POSITION);
}

//デストラクタ
ui::UiMng::~UiMng()
{
	if (this->screen_ != nullptr) {
		delete this->screen_;
	}
	if (this->localImage_ != nullptr) {
		delete this->localImage_;
	}
}

//タスク開始有無
bool ui::UiMng::isStart()
{
	return (this->isStart_ == std::EN_OffOn::ON) ? true : false;
}

//開始
void ui::UiMng::start()
{
	this->isStart_ = std::EN_OffOn::ON;
	this->mainThread_ = std::thread(&main_uimng_task, this);
	this->drawThread_ = std::thread(&main_draw_task, this, this->screen_);
}

//終了
void ui::UiMng::end()
{
	this->isStart_ = std::EN_OffOn::OFF;
	this->mainThread_.join();
	this->drawThread_.join();
}

//ボタンイベント処理
void ui::UiMng::procButtonEvent(const ui::EN_ButtonEvent buttonEvent, const std::CoordI buttonPos)
{
	switch (buttonEvent) {
	case EN_ButtonEvent::LEFT_DOWN:		this->screen_->procButtonLeftDown(buttonPos);	break;
	case EN_ButtonEvent::LEFT_UP:		this->screen_->procButtonLeftUp();				break;
	case EN_ButtonEvent::MOVE:			this->screen_->procButtonMove(buttonPos);		break;
	default:																			break;
	}
}
