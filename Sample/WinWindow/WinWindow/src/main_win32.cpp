#include <Windows.h>
#include <tchar.h>

#include <EGL/egl.h>

#include <cstdio>
#include <string>

#include "WindowIFWIN32.hpp"
#include "DrawGL.hpp"
#include "ImageDecorder.hpp"

//定数定義
namespace {

	//ウィンドウ名
	static const TCHAR* D_WIN_NAME = TEXT("WinWindow");

	//ウィンドウ位置
	static const int D_WIN_POSX = 0;
	static const int D_WIN_POSY = 0;

	//ウィンドウ幅高さ
	static const int D_WIN_WIDTH = 800;
	static const int D_WIN_HEIGHT = 400;

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

	//EGLPBuffer属性
	EGLint attr_pbuffer[] = {
		EGL_WIDTH,		200,
		EGL_HEIGHT,		200,
		EGL_NONE,
	};

	//EGLコンテキスト属性
	EGLint attr_context[] = {
		EGL_CONTEXT_CLIENT_VERSION,	2,
		EGL_NONE,
	};
}

//サンプル描画
namespace {

	//背景色
	draw::ColorUB backColor = { 125, 125, 125, 255 };

	//ライン
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

	//ポリゴン
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

	//画像
	struct ImgFile {
		std::string				filePath;
		draw::EN_ImageFormat	format;
	};
	const std::int32_t imgFileNum = 1;
	ImgFile imgFile[imgFileNum] = {
		{ "./data/bitmap/win-8.bmp", draw::EN_ImageFormat::BMP },
	};
	std::float32_t imgPoint[imgFileNum * 3] = {
		150.0F, 150.0F, 0.0F,
	};
	bool makeImg = false;
	draw::ImageDecorder imgDecoder[imgFileNum];

	//サンプル描画
	void drawSample(window::WindowIF* windowIF, draw::DrawIF* drawIF)
	{
		printf("<drawSample>\n");

		if (!makeImg) {
			for (std::int32_t iImg = 0; iImg < imgFileNum; iImg++) {
				imgDecoder[iImg].decode_RGBA8888(imgFile[iImg].filePath.c_str(), imgFile[iImg].format);
			}
		}

		draw::AreaI area = { 0, 0, 0, 0 };
		windowIF->getWindowSize(&area.xmax, &area.ymax);

		windowIF->makeCurrent(true);
		drawIF->setup(area);
		drawIF->clear(backColor);
		drawIF->drawLines(linePointNum, &linePoints[0], &lineColors[0], 10.0F, draw::EN_LineType::LINE_STRIP);
		drawIF->drawPolygons(polygonPointNum, &polygonPoints[0], &polygonColors[0], draw::EN_PolygonType::TRIANGLE_STRIP);
		for (std::int32_t iImg = 0; iImg < imgFileNum; iImg++) {
			std::int32_t dataSize, w, h;
			std::uint8_t* data = imgDecoder[iImg].getDecodeData(&dataSize, &w, &h);
			draw::ImageAttr imgAttr;
			imgAttr.id = 0;
			imgAttr.width = w;
			imgAttr.height = h;
			imgAttr.pixFormat = draw::EN_PixelFormat::RGBA;
			imgAttr.baseLoc = draw::EN_BaseLoc::CENTER_CENTER;
			drawIF->drawImage(&imgPoint[0], data, imgAttr);
		}
		windowIF->swapBuffers();
		windowIF->makeCurrent(false);
	}
}

//内部関数
namespace {

	//ウィンドウオブジェクト
	class Window;
	Window* gWindow = nullptr;

	//ウィンドウプロシージャ
	LRESULT CALLBACK winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
	 * ウィンドウクラス
	 */
	class Window {
		//メンバ変数
		HINSTANCE		hInstance_;
		HWND			hWnd_;
		EGLDisplay		eglDpy_;
		EGLConfig		eglCfg_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;
		window::WindowIF*	windowIF_;
		draw::DrawIF*		drawIF_;

	public:
		//コンストラクタ
		Window() :
			hInstance_(nullptr), hWnd_(nullptr), eglDpy_(EGL_NO_DISPLAY), eglCfg_(), eglWin_(EGL_NO_SURFACE), eglCtx_(EGL_NO_CONTEXT),
			windowIF_(nullptr), drawIF_(nullptr)
		{
		}

		//デストラクタ
		~Window()
		{
			this->destroy();
		}

		//ウィンドウ作成
		int create()
		{
			printf("<Window::create>\n");

			int rc = -1;

			//インスタンスハンドル取得
			this->hInstance_ = ::GetModuleHandle(nullptr);
			printf("GetModuleHandle %p\n", this->hInstance_);

			//クラス登録情報設定
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = winproc;
			wcex.lpszClassName = D_WIN_NAME;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = this->hInstance_;
			wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
			wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = nullptr;
			wcex.lpszMenuName = nullptr;
			wcex.hIconSm = ::LoadIcon(nullptr, IDI_APPLICATION);

			//クラス登録
			ATOM ret = ::RegisterClassEx(&wcex);
			if (ret == 0) {
				printf("[ERROR] %s:%d RegisterClassEx\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("RegisterClassEx\n");

			//ウィンドウ作成
			HWND hWnd = ::CreateWindowEx(
				0,
				D_WIN_NAME,
				D_WIN_NAME,
				WS_OVERLAPPEDWINDOW,
				D_WIN_POSX,
				D_WIN_POSY,
				D_WIN_WIDTH,
				D_WIN_HEIGHT,
				nullptr,
				nullptr,
				this->hInstance_,
				nullptr
			);
			if (hWnd == nullptr) {
				printf("[ERROR] %s:%d CreateWindowEx\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("CreateWindowEx %p\n", hWnd);

			// ウィンドウ表示
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);

			//正常終了
			rc = 0;

		END:
			return rc;
		}

		//ウィンドウ破棄
		void destroy()
		{
			printf("<Window::destroy>\n");
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
			if (this->hWnd_ != nullptr) {
				::DestroyWindow(this->hWnd_);
			}
		}

		//ウィンドウ開始
		int start()
		{
			printf("<Window::start>\n");

			MSG msg;
			while (GetMessage(&msg, nullptr, 0, 0) != 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			return 0;
		}

		//WM_CREATEイベント処理
		void winproc_create(HWND hWnd)
		{
			printf("<winproc_create> hWnd:0x%p\n", hWnd);

			int rc = -1;
			EGLBoolean retEgl = EGL_FALSE;
			EGLint numCfgs = 0;

			//ウィンドウハンドルを保持
			this->hWnd_ = hWnd;

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

			// EGLコンフィグを取得
			retEgl = eglChooseConfig(this->eglDpy_, attr_config, &this->eglCfg_, 1, &numCfgs);
			if ((retEgl != EGL_TRUE) || (numCfgs != 1)) {
				//失敗
				printf("[ERROR] %s:%d eglChooseConfig %d\n", __FUNCTION__, __LINE__, numCfgs);
				goto END;
			}
			printf("eglChooseConfig %p %d\n", this->eglCfg_, numCfgs);

			//EGLウィンドウサーフェイスを作成
			this->eglWin_ = eglCreateWindowSurface(this->eglDpy_, this->eglCfg_, (NativeWindowType)this->hWnd_, nullptr);
			if (this->eglWin_ == EGL_NO_SURFACE) {
				//失敗
				printf("[ERROR] %s:%d eglCreateWindowSurface\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("eglCreateWindowSurface %p\n", this->eglWin_);

			//EGLPBufferサーフェイスを作成
			EGLSurface pbuf = eglCreatePbufferSurface(this->eglDpy_, this->eglCfg_, attr_pbuffer);
			if (pbuf == EGL_NO_SURFACE) {
				//失敗
				printf("[ERROR] %s:%d eglCreatePbufferSurface\n", __FUNCTION__, __LINE__);
			}
			printf("eglCreatePbufferSurface %p\n", pbuf);

			//EGLコンテキストを作成
			this->eglCtx_ = eglCreateContext(this->eglDpy_, this->eglCfg_, EGL_NO_CONTEXT, attr_context);
			if (this->eglCtx_ == EGL_NO_CONTEXT) {
				//失敗
				printf("[ERROR] %s:%d eglCreateContext\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("eglCreateContext %p\n", this->eglCtx_);

			//ウィンドウインターフェース作成
			this->windowIF_ = new window::WindowIFWIN32(this->hWnd_, this->eglDpy_, this->eglWin_, this->eglCtx_);
			printf("new WindowIFX %p\n", this->windowIF_);

			//描画インターフェース作成(カレント状態が必須)
			this->windowIF_->makeCurrent(true);
			this->drawIF_ = new draw::DrawGL();
			this->windowIF_->makeCurrent(false);
			printf("new DrawGL %p\n", this->drawIF_);

			//正常終了
			rc = 0;

		END:
			if (rc < 0) {
				this->destroy();
			}
		}

		//WM_DESTROYイベント処理
		void winproc_destroy()
		{
			::PostQuitMessage(0);
		}

		//WM_PAINTイベント処理
		void winproc_paint()
		{
			PAINTSTRUCT ps;
			::BeginPaint(this->hWnd_, &ps);
			::EndPaint(this->hWnd_, &ps);

			//サンプル描画
			drawSample(this->windowIF_, this->drawIF_);
		}

		//WM_LBUTTONDOWNイベント処理
		void winproc_lbuttondown(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			printf("<winproc_lbuttondown> x:%d y:%d\n", x, y);
		}

		//WM_LBUTTONUPイベント処理
		void winproc_lbuttonup(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			printf("<winproc_lbuttonup> x:%d y:%d\n", x, y);
		}

		//WM_MOUSEWHEELイベント処理
		void winproc_mousewheel(WPARAM wParam, LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			//回転量を取得(120の倍数、正値は奥、負値は手前)
			int zdelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (zdelta >= 0) {
				//ホイール処理(奥)
				printf("<winproc_mousewheel>FORWARD x:%d y:%d zdelta:%d\n", x, y, zdelta);
			}
			else {
				//ホイール処理(手前)
				printf("<winproc_mousewheel>BACKWARD x:%d y:%d zdelta:%d\n", x, y, zdelta);
			}
		}

		//WM_MOUSEMOVEイベント処理
		void winproc_mousemove(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			//タッチ移動
			//printf("<winproc_mousemove> x:%d y:%d\n", x, y);
		}
	};

	//ウィンドウプロシージャ
	LRESULT CALLBACK winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT ret = 0L;

		switch (msg) {
		case WM_CREATE:
			gWindow->winproc_create(hWnd);
			break;

		case WM_DESTROY:
			gWindow->winproc_destroy();
			break;

		case WM_PAINT:
			gWindow->winproc_paint();
			break;

		case WM_LBUTTONDOWN:
			gWindow->winproc_lbuttondown(lParam);
			break;

		case WM_LBUTTONUP:
			gWindow->winproc_lbuttonup(lParam);
			break;

		case WM_MOUSEWHEEL:
			gWindow->winproc_mousewheel(wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			gWindow->winproc_mousemove(lParam);
			break;

		default:
			//デフォルト処理
			ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}

		return ret;
	}
}

int main()
{
	//コンソールウィンドウ生成
	::AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//ウインドウ作成
	gWindow = new Window();
	int rc = gWindow->create();
	if (rc == 0) {
		//ウィンドウ開始
		rc = gWindow->start();
	}

	//ウインドウ破棄
	delete gWindow;

	//コンソールウィンドウ破棄
	fclose(fConsole);
	//::FreeConsole();	//x64で例外発生

	return rc;
}
