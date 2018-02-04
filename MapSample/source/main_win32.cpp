#include "Std.hpp"
#include "ui/UiMain.hpp"
#include "draw/DrawWGL.hpp"
#include "draw/DrawWEGL.hpp"
#include <Windows.h>
#include <string>


//定数定義
namespace {

	//ウィンドウ名、位置、幅高さ
	static const std::string D_WINDOW_NAME = "MapSample";
	static const std::Position D_WINDOW_POSITION = { 0, 0 };
	static const std::WH D_WINDOW_WH = { 800, 400 };
}

//グローバル変数
namespace {

	//描画インターフェース
	static fw::DrawIF* gDrawIF = nullptr;

	//UiMainオブジェクト
	static ui::UiMain* gUiMain = nullptr;
}

//関数
namespace {

	//WM_CREATEイベント処理
	static void winproc_create(HWND hWnd)
	{
		//描画インターフェース生成
#ifdef DRAWIF_WGL
		gDrawIF = new fw::DrawWGL(hWnd);
#endif //DRAWIF_WGL
#ifdef DRAWIF_WEGL
		gDrawIF = new fw::DrawWEGL(hWnd);
#endif //DRAWIF_WEGL
		gDrawIF->create();

		//UiMainオブジェクト生成
		gUiMain = new ui::UiMain(gDrawIF);

		printf("[%s] hWnd:0x%p DrawIF:0x%p UiMain:0x%p\n", __FUNCTION__, hWnd, gDrawIF, gUiMain);
	}

	//WM_DESTROYイベント処理
	static void winproc_destroy()
	{
		if (gUiMain != nullptr) {
			delete gUiMain;
		}
		if (gDrawIF != nullptr) {
			delete gDrawIF;
		}

		::PostQuitMessage(0);
	}

	//WM_LBUTTONDOWNイベント処理
	static void winproc_lbuttondown(LPARAM lParam)
	{
		//座標を取得
		std::Position buttonPos = { 0 };
		buttonPos.x = std::int32_t(LOWORD(lParam));
		buttonPos.y = std::int32_t(HIWORD(lParam));

		//ボタンイベント処理
		//printf("[%s] WM_LBUTTONDOWN:(%d, %d)\n", __FUNCTION__, buttonPos.x, buttonPos.y);
		gUiMain->procButtonEvent(ui::EN_ButtonEvent::LEFT_DOWN, buttonPos);
	}

	//WM_LBUTTONUPイベント処理
	static void winproc_lbuttonup(LPARAM lParam)
	{
		//座標を取得
		std::Position buttonPos = { 0 };
		buttonPos.x = std::int32_t(LOWORD(lParam));
		buttonPos.y = std::int32_t(HIWORD(lParam));

		//ボタンイベント処理
		gUiMain->procButtonEvent(ui::EN_ButtonEvent::LEFT_UP, buttonPos);
	}

	//WM_MOUSEWHEELイベント処理
	static void winproc_mousewheel(WPARAM wParam, LPARAM lParam)
	{
		//座標を取得
		std::Position buttonPos = { 0 };
		buttonPos.x = std::int32_t(LOWORD(lParam));
		buttonPos.y = std::int32_t(HIWORD(lParam));

		//回転量を取得(120の倍数、正値は奥、負値は手前)
		std::int32_t zdelta = std::int32_t(GET_WHEEL_DELTA_WPARAM(wParam));

		if (zdelta >= 0) {
			//ホイール処理(奥)
			gUiMain->procButtonEvent(ui::EN_ButtonEvent::WHEEL_FORWARD, buttonPos);
		}
		else {
			//ホイール処理(手前)
			gUiMain->procButtonEvent(ui::EN_ButtonEvent::WHEEL_BACKWARD, buttonPos);
		}
	}

	//WM_MOUSEMOVEイベント処理
	static void winproc_mousemove(LPARAM lParam)
	{
		//座標を取得
		std::Position buttonPos = { 0 };
		buttonPos.x = std::int32_t(LOWORD(lParam));
		buttonPos.y = std::int32_t(HIWORD(lParam));

		//タッチ移動
		//printf("[%s] WM_MOUSEMOVE:(%d, %d)\n", __FUNCTION__, buttonPos.x, buttonPos.y);
		gUiMain->procButtonEvent(ui::EN_ButtonEvent::MOVE, buttonPos);
	}

	// ウィンドウプロシージャ
	static LRESULT CALLBACK winproc_main(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT ret = 0L;

		switch (msg) {
		case WM_CREATE:
			winproc_create(hWnd);
			break;

		case WM_DESTROY:
			winproc_destroy();
			break;

		case WM_LBUTTONDOWN:
			winproc_lbuttondown(lParam);
			break;

		case WM_LBUTTONUP:
			winproc_lbuttonup(lParam);
			break;

		case WM_MOUSEWHEEL:
			winproc_mousewheel(wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			winproc_mousemove(lParam);
			break;

		default:
			//デフォルト処理
			ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}

		return ret;
	}
}

//メイン処理
std::int32_t main()
{
	//コンソールウィンドウ生成
	::AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//インスタンスハンドル取得
	HINSTANCE hInstance = ::GetModuleHandle(nullptr);

	//クラス登録情報設定
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = winproc_main;
	wcex.lpszClassName = TEXT(D_WINDOW_NAME.c_str());
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
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
		TEXT(D_WINDOW_NAME.c_str()),
		TEXT(D_WINDOW_NAME.c_str()),
		WS_OVERLAPPEDWINDOW,
		D_WINDOW_POSITION.x,
		D_WINDOW_POSITION.y,
		D_WINDOW_WH.width,
		D_WINDOW_WH.height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	if (hWnd == nullptr) {
		printf("[%s] Failed to CreateWindow\n", __FUNCTION__);
		goto END;
	}

	// ウィンドウ表示
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

END:
	//コンソールウィンドウ破棄
	fclose(fConsole);
	//::FreeConsole();	//x64で例外発生

	return 0;
}