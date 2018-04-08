#include "WindowX.hpp"
#include "DrawGLX.hpp"
#include <cstdio>


namespace {

	draw::ColorUB backColor = { 0, 255, 0, 255 };
	draw::ColorUB lineColor = { 255, 0, 0, 255 };
	const std::int32_t linePointNum = 4;
	draw::PointF linePoints[linePointNum] = {
		0.0F, 0.0F,
		50.0F, 50.0F,
		150.0F, 100.0F,
		300.0F, 0.0F,
	};

	void drawSample(draw::DrawIF* drawIF)
	{
		printf("drawSample\n");
		drawIF->makeCurrent(true);
		drawIF->setup();
		drawIF->clear(backColor);
		drawIF->drawLines(lineColor, &linePoints[0], linePointNum, 10.0F);
		drawIF->swapBuffers();
		drawIF->makeCurrent(false);
	}
};


namespace window {

	//コンストラクタ
	WindowX::WindowX() :
		dpy_(nullptr), win_(0), ctx_(nullptr), drawIF_(nullptr)
	{
	}

	//デストラクタ
	WindowX::~WindowX()
	{
		if (this->drawIF_ != nullptr) {
			delete this->drawIF_;
		}
		if (this->ctx_ != nullptr) {
			glXDestroyContext(this->dpy_, this->ctx_);
		}
		if (this->dpy_ != nullptr) {
			XDestroyWindow(this->dpy_, this->win_);
			XCloseDisplay(this->dpy_);
		}
	}

	//ウィンドウ作成
	int WindowX::create()
	{
		int attr[] = {
			GLX_RGBA,
			GLX_DEPTH_SIZE,
			24,
			GLX_DOUBLEBUFFER,
			None
		};

		//Xサーバ接続
		Display *dpy = XOpenDisplay(nullptr);
		if (dpy == nullptr) {
			//失敗
			printf("XOpenDisplay NOT opened\n");
		}

		Window root = DefaultRootWindow(dpy);

		XVisualInfo *vi = glXChooseVisual(dpy, 0, attr);
		if (vi == nullptr) {
			printf("XVisualInfo NOT found\n");
		}
		else {
			printf("XVisualInfo %p selected\n", (void *)vi->visualid);
		}

		XSetWindowAttributes swAttr;
		swAttr.colormap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swAttr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask;

		//Xウィンドウ作成
		Window win = XCreateWindow(
			dpy,
			root,
			0,
			0,
			300,
			300,
			0,
			vi->depth,
			InputOutput,
			vi->visual,
			CWColormap | CWEventMask,
			&swAttr
		);
		printf("XCreateWindow %d create\n", (int)win);

		//描画コンテキスト作成
		GLXContext ctx = glXCreateContext(dpy, vi, nullptr, True);
		printf("glXCreateContext %p create\n", (void *)ctx);

		//ウィンドウマップ
		XMapWindow(dpy, win);
		//ウィンドウを左上に移動
		XMoveWindow(dpy, win, 0, 0);
		//ウィンドウを最前面に移動
		XRaiseWindow(dpy, win);
		//ウィンドウ名更新
		XStoreName(dpy, win, "XWindow Sample");

		//描画インターフェース作成
		this->drawIF_ = new draw::DrawGLX(dpy, win, ctx);

		this->dpy_ = dpy;
		this->win_ = win;
		this->ctx_ = ctx;

		return 0;
	}

	//ウィンドウ開始
	int WindowX::start()
	{
		printf("WindowX::start\n");
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
				drawSample(this->drawIF_);
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
