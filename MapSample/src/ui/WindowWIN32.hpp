#ifndef INCLUDED_WINDOWWIN32_HPP
#define INCLUDED_WINDOWWIN32_HPP

#include "Window.hpp"
#include "draw/DrawWGL.hpp"
#include "UiMng.hpp"
#include <Windows.h>
#include <new>

namespace ui
{
	//----------------------------------------------------------
	//
	// WGLウィンドウクラス
	//
	//----------------------------------------------------------

	class WindowWIN32 : public Window {
		//メンバ変数
		FILE*			fConsole_;
		HINSTANCE		hInstance_;
		fw::DrawIF*		drawIF_;
		ui::UiMng		uiMng_;

	public:
		//コンストラクタ
		WindowWIN32();
		//デストラクタ
		virtual ~WindowWIN32();
		//ウィンドウ作成
		virtual bool create(const std::string& name, const std::Position pos, const std::WH wh);
		//ウィンドウ開始
		virtual void start();
	};
}

#endif //INCLUDED_WINDOWWIN32_HPP
