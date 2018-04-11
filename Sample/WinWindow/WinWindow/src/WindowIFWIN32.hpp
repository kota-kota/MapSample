#ifndef INCLUDED_WINDOWIFWIN32_HPP
#define INCLUDED_WINDOWIFWIN32_HPP

#include "WindowIF.hpp"
#include <Windows.h>
#include <EGL/egl.h>

namespace window {

	/**
	 * Xウィンドウ操作インターフェースクラス
	 */
	class WindowIFWIN32 : public WindowIF {
		//メンバ変数
		HWND			hWnd_;
		EGLDisplay		eglDpy_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;

	public:
		//コンストラクタ
		WindowIFWIN32(HWND hWnd, EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx);
		//デストラクタ
		virtual ~WindowIFWIN32();
		//ウィンドウサイズ取得
		virtual void getWindowSize(std::int32_t* const width, std::int32_t* const height);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
	};
}

#endif //INCLUDED_WINDOWIFWIN32_HPP
