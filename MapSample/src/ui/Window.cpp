#include "Window.hpp"
#include "WindowWIN32.hpp"


//----------------------------------------------------------
//
// ウィンドウファクトリクラス
//
//----------------------------------------------------------

//コンストラクタ
ui::WindowFactory::WindowFactory() :
	window_(nullptr)
{
}

//デストラクタ
ui::WindowFactory::~WindowFactory()
{
	if (this->window_ != nullptr) {
		delete this->window_;
	}
}

//ウィンドウクラス作成
ui::Window* ui::WindowFactory::create(const EN_WindowType type)
{
	if (this->window_ == nullptr) {
		//ウィンドウクラス未作成の場合

		//ウィンドウタイプに応じたクラスを作成
		switch (type) {
		case WIN32_WGL_GL1:		this->window_ = new WindowWIN32();	break;
		default:				this->window_ = nullptr;			break;
		}
	}
	else {
		//ウィンドウクラス作成済みの場合
		//何もしない
	}

	return this->window_;
}
