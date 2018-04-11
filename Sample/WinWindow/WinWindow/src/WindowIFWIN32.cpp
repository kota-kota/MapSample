#include "WindowIFWIN32.hpp"

namespace window {
	
	/**
	 * WIN32ウィンドウ操作インターフェースクラス
	 */

	//コンストラクタ
	WindowIFWIN32::WindowIFWIN32(HWND hWnd, EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx) :
		hWnd_(hWnd), eglDpy_(eglDpy), eglWin_(eglWin), eglCtx_(eglCtx)
	{
	}

	//デストラクタ
	WindowIFWIN32::~WindowIFWIN32()
	{
	}

	//ウィンドウサイズ取得
	void WindowIFWIN32::getWindowSize(std::int32_t* const width, std::int32_t* const height)
	{
		RECT rect;
		::GetClientRect(this->hWnd_, &rect);

		if(width != nullptr) { *width = rect.right - rect.left; }
		if(height != nullptr) { *height = rect.bottom - rect.top; }
	}

	//描画カレント
	void WindowIFWIN32::makeCurrent(const bool current)
	{
		if (current) {
			eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
		}
		else {
			eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		}
	}

	//描画更新
	void WindowIFWIN32::swapBuffers()
	{
		eglSwapBuffers(this->eglDpy_, this->eglWin_);
	}
}
