#include "WindowCreaterX.hpp"
#include "WindowIFX.hpp"
#include "DrawGL.hpp"
#include <cstdio>


namespace {

	//EGLコンフィグ属性
	EGLint attr_config[] = {
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
	EGLint attr_context[] = {
		EGL_CONTEXT_CLIENT_VERSION,	2,
		EGL_NONE,
	};

	draw::ColorUB backColor = { 125, 125, 125, 255 };
	const std::int32_t linePointNum = 4;
	std::float32_t linePoints[linePointNum * 3] = {
		0.0F, 0.0F, 0.0F,
		50.0F, 50.0F, 0.0F,
		150.0F, 100.0F, 0.0F,
		300.0F, 0.0F, 0.0F,
	};
	std::uint8_t lineColors[linePointNum * 4] = {
		255, 0, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
	};
	const std::int32_t polygonPointNum = 3;
	std::float32_t polygonPoints[polygonPointNum * 3] = {
		150.0F, 150.0F, 0.0F,
		200.0F, 200.0F, 0.0F,
		240.0F, 150.0F, 0.0F,
	};
	std::uint8_t polygonColors[polygonPointNum * 4] = {
		255, 0, 0, 255,
		0, 255, 0, 255,
		0, 0, 255, 255,
	};

	void drawSample(window::WindowIF* windowIF, draw::DrawIF* drawIF)
	{
		printf("drawSample\n");

		draw::AreaI area = {0, 0, 0, 0};
		windowIF->getWindowSize(&area.xmax, &area.ymax);

		windowIF->makeCurrent(true);
		drawIF->setup(area);
		drawIF->clear(backColor);
		drawIF->drawLines(linePointNum, &linePoints[0], &lineColors[0], 10.0F, draw::EN_LineType::LINE_STRIP);
		drawIF->drawPolygons(polygonPointNum, &polygonPoints[0], &polygonColors[0], draw::EN_PolygonType::TRIANGLE_STRIP);
		windowIF->swapBuffers();
		windowIF->makeCurrent(false);
	}
};


namespace window {

	/**
	 * Xウィンドウ作成クラス
	 */

	//コンストラクタ
	WindowCreaterX::WindowCreaterX() :
		dpy_(nullptr), win_(0), eglDpy_(EGL_NO_DISPLAY), eglCfg_(), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT),
		windowIF_(nullptr), drawIF_(nullptr)
	{
	}

	//デストラクタ
	WindowCreaterX::~WindowCreaterX()
	{
		this->destroy();
	}

	//ウィンドウ作成
	int WindowCreaterX::create()
	{
		printf("<WindowCreaterX::create>\n");
		EGLBoolean retEgl = EGL_FALSE;
		EGLint numCfgs = 0;
		long eventMask = 0;

		//X11をスレッドセーフにする
		XInitThreads();

		//Xサーバ接続
		this->dpy_ = XOpenDisplay(nullptr);
		if (this->dpy_ == nullptr) {
			//失敗
			printf("[ERROR] %s:%d XOpenDisplay\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("XOpenDisplay %p\n", this->dpy_);

		//Xウィンドウ作成
		this->win_ = XCreateSimpleWindow(
			this->dpy_,
			DefaultRootWindow(this->dpy_),
			0,
			0,
			300,
			300,
			0,
			BlackPixel(this->dpy_, 0),
			WhitePixel(this->dpy_, 0)
		);
		if(this->win_ == 0) {
			//失敗
			printf("[ERROR] %s:%d XCreateWindow\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("XCreateWindow %d\n", (int)this->win_);

		//サーバに通知するイベントを選択
		eventMask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask;
		XSelectInput(this->dpy_, this->win_, eventMask);
		//ウィンドウマップ
		XMapWindow(this->dpy_, this->win_);
		//ウィンドウを左上に移動
		XMoveWindow(this->dpy_, this->win_, 0, 0);
		//ウィンドウを最前面に移動
		XRaiseWindow(this->dpy_, this->win_);
		//ウィンドウ名更新
		XStoreName(this->dpy_, this->win_, "XWindow Sample");

		//EGLディスプレイを取得
		this->eglDpy_ = eglGetDisplay((NativeDisplayType)this->dpy_);
		if(this->eglDpy_ == EGL_NO_DISPLAY) {
			//失敗
			printf("[ERROR] %s:%d eglGetDisplay\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("eglGetDisplay %p\n", this->eglDpy_);

		//EGL初期化
		EGLint major, minor;
		retEgl = eglInitialize(this->eglDpy_, &major, &minor);
		if(retEgl != EGL_TRUE) {
			//失敗
			printf("[ERROR] %s:%d eglInitialize\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("eglInitialize EGL%d.%d\n", major, minor);

		// EGLコンフィグを取得
		retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numCfgs);
		if((retEgl != EGL_TRUE) || (numCfgs != 1)) {
			//失敗
			printf("[ERROR] %s:%d eglChooseConfig %d\n", __FUNCTION__, __LINE__, numCfgs);
			goto ERR;
		}
		printf("eglChooseConfig %p %d\n", this->eglCfg_, numCfgs);

		//EGLウィンドウサーフェイスを作成
		this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)this->win_, nullptr);
		if(this->eglWin_ == EGL_NO_SURFACE) {
			//失敗
			printf("[ERROR] %s:%d eglCreateWindowSurface\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("eglCreateWindowSurface %p\n", this->eglWin_);

		//EGLコンテキストを作成
		this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
		if(this->eglCtx_ == EGL_NO_CONTEXT) {
			//失敗
			printf("[ERROR] %s:%d eglCreateContext\n", __FUNCTION__, __LINE__);
			goto ERR;
		}
		printf("eglCreateContext %p\n", this->eglCtx_);

		//ウィンドウインターフェース作成
		this->windowIF_ = new WindowIFX(this->dpy_, this->win_, this->eglDpy_, this->eglWin_, this->eglCtx_);
		printf("new WindowIFX %p\n", this->windowIF_);

		//描画インターフェース作成(カレント状態が必須)
		this->windowIF_->makeCurrent(true);
		this->drawIF_ = new draw::DrawGL();
		this->windowIF_->makeCurrent(false);
		printf("new DrawGL %p\n", this->drawIF_);

		//作成成功
		return 0;

	ERR:
		//作成失敗
		//途中まで作成した資源を破棄
		this->destroy();
		return -1;
	}

	//ウィンドウ破棄
	void WindowCreaterX::destroy()
	{
		printf("<WindowCreaterX::destroy>\n");
		if (this->drawIF_ != nullptr) {
			printf("delete DrawIF %p\n", this->drawIF_);
			delete this->drawIF_;
		}
		if (this->windowIF_ != nullptr) {
			printf("delete WindowIF %p\n", this->windowIF_);
			delete this->windowIF_;
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
		if (this->win_ != 0) {
			printf("XDestroyWindow %d\n", (int)this->win_);
			XDestroyWindow(this->dpy_, this->win_);
		}
		if (this->dpy_ != nullptr) {
			printf("XCloseDisplay %p\n", this->dpy_);
			XCloseDisplay(this->dpy_);
		}
	}

	//ウィンドウ開始
	int WindowCreaterX::start()
	{
		printf("<WindowCreaterX::start>\n");
		bool running = true;

		//アトム名設定(閉じるボタン処理用)
		Atom atom1 = XInternAtom(this->dpy_, "WM_PROTOCOLS", False);
		Atom atom2 = XInternAtom(this->dpy_, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(this->dpy_, this->win_, &atom2, 1);

		while (running) {
			XEvent ev;
			XNextEvent(this->dpy_, &ev);

			switch (ev.type) {
			case Expose:
			{
				printf("EVENT: Expose\n");
				drawSample(this->windowIF_, this->drawIF_);
				break;
			}
			case ButtonPress:
			{
				//押下された画面座標(左上基準)
				std::int32_t x = ev.xbutton.x;
				std::int32_t y = ev.xbutton.y;

				if (ev.xbutton.button == 1) {
					//左
					printf("EVENT: ButtonPress <Left>(%d %d)\n", x, y);
				}
				else if (ev.xbutton.button == 2) {
					//ホイール
					printf("EVENT: ButtonPress <Wheel>(%d %d)\n", x, y);
				}
				else if (ev.xbutton.button == 3) {
					//右
					printf("EVENT: ButtonPress <Right>(%d %d)\n", x, y);
				}
				else if (ev.xbutton.button == 4) {
					//ホイールアップ
					printf("EVENT: ButtonPress <WheelUp>(%d %d)\n", x, y);
				}
				else if (ev.xbutton.button == 5) {
					//ホイールダウン
					printf("EVENT: ButtonPress <WheelDown>(%d %d)\n", x, y);
				}
				else {
					//未サポート
				}

				break;
			}
			case ButtonRelease:
			{
				printf("EVENT: ButtonRelease\n");
				break;
			}
			case MotionNotify:
			{
				//printf("EVENT: MotionNotify\n");
				break;
			}
			case KeyPress:
			{
				KeySym keySym = XkbKeycodeToKeysym(this->dpy_, ev.xkey.keycode, 0, (ev.xkey.state & ShiftMask ? 1 : 0));
				if (keySym == XK_Up) {
					//↑キー
					printf("EVENT: KeyPress <XK_Up>\n");
				}
				else if (keySym == XK_Down) {
					//↓キー
					printf("EVENT: KeyPress <XK_Down>\n");
				}
				else if (keySym == XK_Escape) {
					//ESCキー
					printf("EVENT: KeyPress <XK_Escape>\n");
				}
				else if ((keySym == XK_Shift_L) || (keySym == XK_Shift_R)) {
					//Shiftキー
					printf("EVENT: KeyPress <Shift>\n");
				}
				else if ((keySym == XK_Control_L) || (keySym == XK_Control_R)) {
					//Ctrlキー
					printf("EVENT: KeyPress <Ctrl>\n");
				}
				else {
					//未サポート
				}
				break;
			}
			case KeyRelease:
			{
				printf("EVENT: KeyRelease\n");
				break;
			}
			case ClientMessage:
			{
				//閉じる釦イベント
				if ((atom1 == ev.xclient.message_type) &&
					(atom2 == static_cast<Atom>(ev.xclient.data.l[0]))) {
					//終了
					running = false;
				}
				break;
			}
			default:
				break;
			};
		}

		return 0;
	}
}
