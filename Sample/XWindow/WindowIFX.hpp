#ifndef INCLUDED_WINDOWIFX_HPP
#define INCLUDED_WINDOWIFX_HPP

#include "WindowIF.hpp"
#include <X11/X.h>
#include <EGL/egl.h>

namespace window {

	/**
	 * Xウィンドウ操作インターフェースクラス
	 */
	class WindowIFX : public WindowIF {
		//メンバ変数
		Display*		dpy_;
		Window			win_;
		EGLDisplay		eglDpy_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;

	public:
		//コンストラクタ
		WindowIFX(Display* dpy, Window win, EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx);
		//デストラクタ
		virtual ~WindowIFX();
		//ウィンドウサイズ取得
		virtual void getWindowSize(std::int32_t* const width, std::int32_t* const height);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
	};
}

#endif //INCLUDED_WINDOWIFX_HPP
