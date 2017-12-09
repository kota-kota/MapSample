#include "Std.hpp"
#include "draw/DrawWGL.hpp"
#include "ui/UiMng.hpp"
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

	//UiMngオブジェクト
	static ui::UiMng* gUiMng = nullptr;
}

//関数
namespace {

	//WM_CREATEイベント処理
	static void winproc_create(HWND hWnd)
	{
		//描画インターフェース生成
		gDrawIF = new fw::DrawWGL(hWnd);
		gDrawIF->create();

		//UiMngオブジェクト生成
		gUiMng = new ui::UiMng(gDrawIF);

		printf("[%s] hWnd:0x%p DrawIF:0x%p UiMng:0x%p\n", __FUNCTION__, hWnd, gDrawIF, gUiMng);
	}

	//WM_DESTROYイベント処理
	static void winproc_destroy()
	{
		if (gUiMng != nullptr) {
			delete gUiMng;
		}
		if (gDrawIF != nullptr) {
			delete gDrawIF;
		}

		::PostQuitMessage(0);
	}

	//WM_PAINTイベント処理
	static void winproc_paint(HWND hWnd)
	{
		printf("%s\n", __FUNCTION__);

		PAINTSTRUCT ps;
		::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);

		//描画
		gUiMng->draw();
	}

	//WM_LBUTTONDOWNイベント処理
	static void winproc_lbuttondown(LPARAM lParam)
	{
		//タッチ座標を取得
		std::CoordI touchPos = { 0, 0, 0 };
		touchPos.x = std::int16_t(LOWORD(lParam));
		touchPos.y = std::int16_t(HIWORD(lParam));

		//タッチON
		printf("[%s] WM_LBUTTONDOWN:(%d, %d)\n", __FUNCTION__, touchPos.x, touchPos.y);
		gUiMng->setTouchOn(touchPos);
	}

	//WM_LBUTTONUPイベント処理
	static void winproc_lbuttonup(HWND hWnd)
	{
		//タッチOFF
		gUiMng->setTouchOff();

		//描画更新イベント通知
		RECT rect;
		::GetClientRect(hWnd, &rect);
		::InvalidateRect(hWnd, &rect, false);
	}

	//WM_MOUSEMOVEイベント処理
	static void winproc_mousemove(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		if (wParam & MK_LBUTTON) {
			//ドラッグ座標を取得
			std::CoordI dragPos = { 0, 0, 0 };
			dragPos.x = std::int16_t(LOWORD(lParam));
			dragPos.y = std::int16_t(HIWORD(lParam));

			//ドラッグ
			printf("[%s] WM_MOUSEMOVE:(%d, %d)\n", __FUNCTION__, dragPos.x, dragPos.y);
			gUiMng->setDrag(dragPos);

			//描画更新イベント通知
			RECT rect;
			::GetClientRect(hWnd, &rect);
			::InvalidateRect(hWnd, &rect, false);
		}
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

		case WM_PAINT:
			winproc_paint(hWnd);
			break;

		case WM_LBUTTONDOWN:
			winproc_lbuttondown(lParam);
			break;

		case WM_LBUTTONUP:
			winproc_lbuttonup(hWnd);

		case WM_MOUSEMOVE:
			winproc_mousemove(hWnd, wParam, lParam);

		default:
			//デフォルト処理
			ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
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