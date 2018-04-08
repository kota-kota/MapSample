#include "WindowIFX.hpp"

namespace window {
	
	/**
	 * Xウィンドウ操作インターフェースクラス
	 */

	//コンストラクタ
	WindowIFX::WindowIFX(Display* dpy, Window win, EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx) :
		dpy_(dpy), win_(win), eglDpy_(eglDpy), eglWin_(eglWin), eglCtx_(eglCtx)
	{
	}

	//デストラクタ
	WindowIFX::~WindowIFX()
	{
	}

	//ウィンドウサイズ取得
	void WindowIFX::getWindowSize(std::int32_t* const width, std::int32_t* const height)
	{
		XWindowAttributes wAttr;
		XGetWindowAttributes(this->dpy_, this->win_, &wAttr);

		if(width != nullptr) { *width = wAttr.width; }
		if(height != nullptr) { *height = wAttr.height; }
	}

	//描画カレント
	void WindowIFX::makeCurrent(const bool current)
	{
		if (current) {
			eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
		}
		else {
			eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		}
	}

	//描画更新
	void WindowIFX::swapBuffers()
	{
		eglSwapBuffers(this->eglDpy_, this->eglWin_);
	}
}
