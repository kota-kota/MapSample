#include "DigitalWatch.hpp"

namespace ui {

	//----------------------------------------------------------
	//
	// デジタル時計クラス実装
	//
	//----------------------------------------------------------

	//コンストラクタ
	DigitalWatch::DigitalWatch(const HWND hWnd) :
		hWnd_(hWnd), drawIF_(nullptr)
	{
		this->drawIF_ = new draw::DrawWGL(hWnd);
		this->drawIF_->create();
	}

	//デストラクタ
	DigitalWatch::~DigitalWatch()
	{
		delete this->drawIF_;
	}

	//描画
	void DigitalWatch::draw()
	{
		//描画開始
		this->drawIF_->makeCurrent(true);

		//描画準備
		this->drawIF_->setup();

		//背景色
		draw::ColorUB backColor = { 125, 125, 125, 255 };
		this->drawIF_->clear(backColor);

		//描画終了
		this->drawIF_->swapBuffers();
		this->drawIF_->makeCurrent(false);
	}
}