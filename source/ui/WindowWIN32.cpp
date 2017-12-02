#include "WindowWIN32.hpp"



namespace {

	//WM_CREATEイベント処理
	static bool winproc_create(HWND hWnd, LPARAM lParam)
	{
		//CreateWindowのパラメータからUiMngを取得
		LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		ui::UiMng* uiMng = static_cast<ui::UiMng*>(lpCreateStruct->lpCreateParams);

		if (uiMng == nullptr) {
			return false;
		}

		//hWndにUiMngを関連付け
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(uiMng));

		printf("[%s] hWnd:0x%p UiMng:0x%p\n", __FUNCTION__, hWnd, uiMng);

		return true;
	}

	//WM_DESTROYイベント処理
	static bool winproc_destroy()
	{
		::PostQuitMessage(0);

		return true;
	}

	//WM_PAINTイベント処理
	static bool winproc_paint(HWND hWnd)
	{
		//hWndに関連付けた値からUiMngを取得
		ui::UiMng* uiMng = reinterpret_cast<ui::UiMng*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		printf("%s\n", __FUNCTION__);

		PAINTSTRUCT ps;
		::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);

		//描画
		uiMng->draw();

		return true;
	}

	//ユーザ操作イベント処理
	static bool winproc_useroperation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//hWndに関連付けた値からUiMngを取得
		ui::UiMng* uiMng = reinterpret_cast<ui::UiMng*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (msg == WM_LBUTTONDOWN) {
			std::CoordI touchPos = { 0, 0, 0 };
			touchPos.x = std::int16_t(LOWORD(lParam));
			touchPos.y = std::int16_t(HIWORD(lParam));
			printf("[%s] WM_LBUTTONDOWN:(%d, %d)\n", __FUNCTION__, touchPos.x, touchPos.y);

			//タッチON
			uiMng->setTouchOn(touchPos);

			return true;
		}
		else if (msg == WM_LBUTTONUP) {
			//タッチOFF
			uiMng->setTouchOff();

			RECT rect;
			::GetClientRect(hWnd, &rect);
			::InvalidateRect(hWnd, &rect, false);

			return true;
		}
		else if (msg == WM_MOUSEMOVE) {
			if (wParam & MK_LBUTTON) {
				std::CoordI dragPos = { 0, 0, 0 };
				dragPos.x = std::int16_t(LOWORD(lParam));
				dragPos.y = std::int16_t(HIWORD(lParam));
				printf("[%s] WM_MOUSEMOVE:(%d, %d)\n", __FUNCTION__, dragPos.x, dragPos.y);

				//ドラッグ
				uiMng->setDrag(dragPos);

				//描画更新イベント通知
				RECT rect;
				::GetClientRect(hWnd, &rect);
				::InvalidateRect(hWnd, &rect, false);

				return true;
			}
			return false;
		}
		else {
			return false;
		}
	}

	// ウィンドウプロシージャ
	static LRESULT CALLBACK winproc_main(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		bool ret = true;

		switch (msg) {
		case WM_CREATE:
			ret = winproc_create(hWnd, lParam);
			break;

		case WM_DESTROY:
			ret = winproc_destroy();
			break;

		case WM_PAINT:
			ret = winproc_paint(hWnd);
			break;

		default:
			ret = winproc_useroperation(hWnd, msg, wParam, lParam);
			if (!ret) {
				//デフォルト処理
				return ::DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}

		return (0L);
	}
}


//----------------------------------------------------------
//
// WIN32ウィンドウクラス
//
//----------------------------------------------------------

//コンストラクタ
ui::WindowWIN32::WindowWIN32()
	: fConsole_(nullptr), hInstance_(nullptr), drawIF_(nullptr), uiMng_()
{
	//コンソールウィンドウ生成
	::AllocConsole();
	freopen_s(&this->fConsole_, "CONOUT$", "w", stdout);

	//インスタンスハンドル取得
	this->hInstance_ = ::GetModuleHandle(nullptr);

	//描画インターフェース領域確保
	this->drawIF_ = new fw::DrawWGL();
}

//デストラクタ
ui::WindowWIN32::~WindowWIN32()
{
	//描画インターフェース領域解放
	if (this->drawIF_ != nullptr) {
		delete this->drawIF_;
	}

	//コンソールウィンドウ破棄
	fclose(this->fConsole_);
	//::FreeConsole();	//x64で例外発生
}

//ウィンドウ作成
bool ui::WindowWIN32::create(const std::string& name, const std::Position pos, const std::WH wh)
{
	bool result = false;

	//クラス名、ウィンドウタイトル
	LPCSTR className = TEXT(name.c_str());
	LPCSTR windowTitle = TEXT(name.c_str());

	//クラス登録情報設定
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = winproc_main;
	wcex.lpszClassName = className;
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
		printf("[%s] Failed to RegisterClassEx\n", __FUNCTION__);
		goto END;
	}

	//ウィンドウ作成
	HWND hWnd = ::CreateWindowEx(
		0,
		className,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		pos.x,
		pos.y,
		wh.width,
		wh.height,
		nullptr,
		nullptr,
		this->hInstance_,
		static_cast<LPVOID>(&this->uiMng_)
	);
	if (hWnd == nullptr) {
		printf("[%s] Failed to CreateWindow\n", __FUNCTION__);
		goto END;
	}

	printf("[%s] DrawWGL:0x%p UiMng:0x%p\n", __FUNCTION__, this->drawIF_, &this->uiMng_);

	//描画インターフェース生成
	new(this->drawIF_) fw::DrawWGL(hWnd);
	this->drawIF_->create();

	//UiMng生成
	new(&this->uiMng_) ui::UiMng(this->drawIF_);

	// ウィンドウ表示
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	result = true;

END:
	return result;
}

//ウィンドウ開始
void ui::WindowWIN32::start()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
