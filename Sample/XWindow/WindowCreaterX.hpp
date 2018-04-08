#ifndef INCLUDED_WINDOWCREATERX_HPP
#define INCLUDED_WINDOWCREATERX_HPP

#include "WindowCreater.hpp"
#include "WindowIF.hpp"
#include "DrawIF.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <EGL/egl.h>

namespace window {

	/**
	 * Xウィンドウ作成クラス
	 */
	class WindowCreaterX : public WindowCreater {
		//メンバ変数
		Display*		dpy_;
		Window			win_;
		EGLDisplay		eglDpy_;
		EGLConfig		eglCfg_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;
		WindowIF*		windowIF_;
		draw::DrawIF*	drawIF_;

	public:
		//コンストラクタ
		WindowCreaterX();
		//デストラクタ
		virtual ~WindowCreaterX();
		//ウィンドウ作成
		virtual int create();
		//ウィンドウ開始
		virtual int start();

	private:
		//ウィンドウ破棄
		void destroy();
	};
}

#endif //INCLUDED_WINDOWCREATERX_HPP
