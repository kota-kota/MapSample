#ifndef INCLUDED_WINDOW_HPP
#define INCLUDED_WINDOW_HPP

#include "Std.hpp"
#include <string>

namespace ui {

	//ウィンドウタイプ
	enum EN_WindowType {
		WIN32_WGL_GL1,
		WIN32_WGL_GL2,
		X11_EGL_GL1,
		X11_EGL_GL2,
	};

	//----------------------------------------------------------
	//
	// ウィンドウクラス
	//
	//----------------------------------------------------------

	class Window {
	public:
		//コンストラクタ
		Window() {}
		//デストラクタ
		virtual ~Window() {}
		//ウィンドウ作成
		virtual bool create(const std::string& name, const std::Position pos, const std::WH wh) = 0;
		//ウィンドウ開始
		virtual void start() = 0;
	};


	//----------------------------------------------------------
	//
	// ウィンドウファクトリクラス
	//
	//----------------------------------------------------------

	class WindowFactory {
		//メンバ変数
		Window* window_;

	public:
		//コンストラクタ
		WindowFactory();
		//デストラクタ
		~WindowFactory();
		//ウィンドウクラス作成
		Window* create(const EN_WindowType type);
	};
}

#endif //INCLUDED_WINDOW_HPP
