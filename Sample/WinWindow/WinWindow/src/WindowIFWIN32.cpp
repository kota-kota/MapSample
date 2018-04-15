#include "WindowIFWIN32.hpp"
#include <cstdio>

namespace window {
	
	/**
	 * WIN32ウィンドウ操作インターフェースクラス
	 */

	//コンストラクタ
	WindowIFWIN32::WindowIFWIN32(HWND hWnd, EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx) :
		hWnd_(hWnd), eglDpy_(eglDpy), eglWin_(eglWin), eglCtx_(eglCtx)
	{
#if 1	//TEST
		//FBO作成

		//カレント設定
		this->makeCurrent(true);

		//ウィンドウの幅高さをFBOの幅高さとする
		std::int32_t width, height;
		this->getWindowSize(&width, &height);

		//カラーバッファ用のテクスチャを用意する
		GLuint cb = 0;
		glGenTextures(1, &cb);
		glBindTexture(GL_TEXTURE_2D, cb);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		printf("[FBO] ColorTextureBufffer %d\n", cb);

		//デプスバッファ用のレンダーバッファを用意する
		GLuint rb = 0;
		glGenRenderbuffers(1, &rb);
		glBindRenderbuffer(GL_RENDERBUFFER, rb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, width);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		printf("[FBO] DepthRenderBufffer %d\n", rb);

		//フレームバッファオブジェクトを作成する
		GLuint fb = 0;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		printf("[FBO] FrameBufffer %d\n", fb);

		//フレームバッファオブジェクトにカラーバッファとしてテクスチャを結合する
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cb, 0);

		//フレームバッファオブジェクトにデプスバッファとしてレンダーバッファを結合する
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

		//フレームバッファオブジェクトの結合を解除する
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//カレント解除
		this->makeCurrent(false);
#endif
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
