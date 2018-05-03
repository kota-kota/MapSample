#include "LayerManager.hpp"
#include "DrawGL.hpp"
#include <cstdio>

namespace fw {

	/**
	* レイヤー操作クラス
	*/
	class Layer : public LayerIF {
		//メンバ変数
		EGLDisplay		eglDpy_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;

	public:
		//コンストラクタ
		Layer(EGLDisplay eglDpy, EGLSurface eglWin, EGLContext eglCtx, const int32_t width, const int32_t height, DrawIF* const drawIF) :
			LayerIF(width, height, drawIF), eglDpy_(eglDpy), eglWin_(eglWin), eglCtx_(eglCtx)
		{
		}

		//デストラクタ
		virtual ~Layer()
		{
		}

		//描画開始
		virtual void beginDraw()
		{
			//サーフェイスに描画コンテキストを紐づけ
			(void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
		}

		//描画終了
		virtual void endDraw()
		{
			//バッファ入れ替え
			(void)eglSwapBuffers(this->eglDpy_, this->eglWin_);

			//描画コンテキストの紐づけを解除
			(void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		}
	};




	/**
	* レイヤー管理クラス
	*/

	//コンストラクタ
	LayerManager::LayerManager() :
		native_(nullptr), eglDpy_(EGL_NO_DISPLAY), eglCfg_(nullptr), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT),
		drawIF_(nullptr), layerList_()
	{
	}

	//デストラクタ
	LayerManager::~LayerManager()
	{
		this->destroyWindow();
	}

	//ウィンドウ作成
	bool LayerManager::createWindow(void* const native)
	{
		bool ret = false;
		EGLBoolean retEgl = EGL_FALSE;
		EGLint numCfgs = 0;

		//ネイティブウィンドウを保持
		this->native_ = native;

		//EGLコンフィグ属性
		const EGLint attr_config[] = {
			EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
			EGL_RED_SIZE,			8,
			EGL_GREEN_SIZE,			8,
			EGL_BLUE_SIZE,			8,
			EGL_ALPHA_SIZE,			8,
			EGL_DEPTH_SIZE,			8,
			EGL_NONE,
		};

		//EGLコンテキスト属性
		const EGLint attr_context[] = {
			EGL_CONTEXT_CLIENT_VERSION,	2,
			EGL_NONE,
		};

		//EGLディスプレイを取得
		this->eglDpy_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if (this->eglDpy_ == EGL_NO_DISPLAY) {
			//失敗
			printf("[ERROR] %s:%d eglGetDisplay\n", __FUNCTION__, __LINE__);
			goto END;
		}
		printf("eglGetDisplay %p\n", this->eglDpy_);

		//EGL初期化
		EGLint major, minor;
		retEgl = eglInitialize(this->eglDpy_, &major, &minor);
		if (retEgl != EGL_TRUE) {
			//失敗
			printf("[ERROR] %s:%d eglInitialize\n", __FUNCTION__, __LINE__);
			goto END;
		}
		printf("eglInitialize EGL%d.%d\n", major, minor);

		//EGLコンフィグを取得
		retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numCfgs);
		if ((retEgl != EGL_TRUE) || (numCfgs != 1)) {
			//失敗
			printf("[ERROR] %s:%d eglChooseConfig %d\n", __FUNCTION__, __LINE__, numCfgs);
			goto END;
		}
		printf("eglChooseConfig %p %d\n", this->eglCfg_, numCfgs);

		//EGLウィンドウサーフェイスを作成
		this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)this->native_, nullptr);
		if (this->eglWin_ == EGL_NO_SURFACE) {
			//失敗
			printf("[ERROR] %s:%d eglCreateWindowSurface\n", __FUNCTION__, __LINE__);
			goto END;
		}
		printf("eglCreateWindowSurface %p\n", this->eglWin_);

		//EGLコンテキストを作成
		this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
		if (this->eglCtx_ == EGL_NO_CONTEXT) {
			//失敗
			printf("[ERROR] %s:%d eglCreateContext\n", __FUNCTION__, __LINE__);
			goto END;
		}
		printf("eglCreateContext %p\n", this->eglCtx_);

		//描画インターフェースを生成
		(void)eglMakeCurrent(this->eglDpy_, this->eglWin_, this->eglWin_, this->eglCtx_);
		this->drawIF_ = new DrawGL();
		printf("new DrawGL %p\n", this->drawIF_);
		(void)eglMakeCurrent(this->eglDpy_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		//正常終了
		ret = true;

	END:
		return ret;
	}

	//ウィンドウ破棄
	void LayerManager::destroyWindow()
	{
		if (this->drawIF_ != nullptr) {
			printf("new DrawGL %p\n", this->drawIF_);
			delete this->drawIF_;
		}
		if (this->eglCtx_ != EGL_NO_CONTEXT) {
			printf("eglDestroyContext %p\n", this->eglCtx_);
			eglDestroyContext(this->eglDpy_, this->eglCtx_);
		}
		if (this->eglWin_ != EGL_NO_SURFACE) {
			printf("eglDestroySurface %p\n", this->eglWin_);
			eglDestroySurface(this->eglDpy_, this->eglWin_);
		}
		if (this->eglDpy_ != EGL_NO_DISPLAY) {
			printf("eglTerminate %p\n", this->eglDpy_);
			eglTerminate(this->eglDpy_);
		}
	}

	//レイヤー作成
	LayerIF* LayerManager::createLayer(const int32_t width, const int32_t height)
	{
		//レイヤーオブジェクトを作成
		LayerIF* layer = new Layer(this->eglDpy_, this->eglWin_, this->eglCtx_, width, height, this->drawIF_);

		//リストに追加
		this->layerList_.push_back(layer);

		return layer;
	}

	//レイヤー破棄
	void LayerManager::destroyLayer(LayerIF** const layer)
	{
		//削除するレイヤーID
		std::int32_t delId = (*layer)->getId();

		for (auto itr = this->layerList_.cbegin(); itr != this->layerList_.cend(); itr++) {
			//レイヤーIDを探す
			if (delId == (*itr)->getId()) {
				//一致したレイヤーを削除
				this->layerList_.erase(itr);
				break;
			}
		}

		*layer = nullptr;
	}


#if 0
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
#endif
}
