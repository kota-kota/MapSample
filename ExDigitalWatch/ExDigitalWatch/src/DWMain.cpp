#include "DWMain.hpp"
#include "DWWindow.hpp"

namespace {
	//DWMainインスタンス
	dw::DWMain* g_dwmain = nullptr;
	//グローバルミューテックス
	std::mutex g_mtx;
}

namespace dw {

	//----------------------------------------------------------------
	// DWMainクラス
	//----------------------------------------------------------------

	//開始
	void DWMain::start()
	{
		//ZDigitalWatchインスタンスが未生成なら生成する
		g_mtx.lock();
		if (g_dwmain == nullptr) {
			g_dwmain = new DWMain();
		}
		g_mtx.unlock();
	}

	//終了
	void DWMain::terminate()
	{
		g_mtx.lock();
		if (g_dwmain != nullptr) {
			delete g_dwmain;
			g_dwmain = nullptr;
		}
		g_mtx.unlock();
	}

	//コンストラクタ
	DWMain::DWMain() :
		th_(), mtx_(), taskState_(END)
	{
		//タスク開始
		this->mtx_.lock();
		this->taskState_ = START;
		this->mtx_.unlock();
		//スレッド作成
		this->th_ = std::thread(&DWMain::task, this);
	}

	//デストラクタ
	DWMain::~DWMain()
	{
		//タスク終了
		this->mtx_.lock();
		this->taskState_ = END;
		this->mtx_.unlock();
		//スレッド破棄
		this->th_.join();
	}

	//メインタスク
	void DWMain::task()
	{
		while (true) {
			//タスク状態を取得
			this->mtx_.lock();
			const bool endTask = (this->taskState_ == END) ? true : false;
			this->mtx_.unlock();

			if (endTask) {
				//タスク終了
				break;
			}

			//描画
			DWWindow* dwwin = DWWindow::get();
			dwwin->beginDraw();
			const DWColor color = { 255.0F, 0.0F, 0.0F, 255.0F };
			dwwin->clear(color);
			DWText text = { 0 };
			text.num_ = 3;
			text.text_[0] = L'1';
			text.text_[1] = L'2';
			text.text_[2] = L':';
			text.text_[3] = L'\0';
			dwwin->drawText(text);
			dwwin->drawBitmap();
			dwwin->endDraw();

			//待ち
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}
