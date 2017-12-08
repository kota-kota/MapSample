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
	static bool winproc_create(HWND hWnd, LPARAM lParam)
	{
		//描画インターフェース生成
		gDrawIF = new fw::DrawWGL(hWnd);
		gDrawIF->create();

		//UiMngオブジェクト生成
		gUiMng = new ui::UiMng(gDrawIF);

		printf("[%s] hWnd:0x%p DrawIF:0x%p UiMng:0x%p\n", __FUNCTION__, hWnd, gDrawIF, gUiMng);
		return true;
	}

	//WM_DESTROYイベント処理
	static bool winproc_destroy()
	{
		if (gUiMng != nullptr) {
			delete gUiMng;
		}
		if (gDrawIF != nullptr) {
			delete gDrawIF;
		}

		::PostQuitMessage(0);
		return true;
	}

	//WM_PAINTイベント処理
	static bool winproc_paint(HWND hWnd)
	{
		printf("%s\n", __FUNCTION__);

		PAINTSTRUCT ps;
		::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);

		if (gUiMng != nullptr) {
			//描画
			gUiMng->draw();
		}

		return true;
	}

	//ユーザ操作イベント処理
	static bool winproc_useroperation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_LBUTTONDOWN) {
			std::CoordI touchPos = { 0, 0, 0 };
			touchPos.x = std::int16_t(LOWORD(lParam));
			touchPos.y = std::int16_t(HIWORD(lParam));
			printf("[%s] WM_LBUTTONDOWN:(%d, %d)\n", __FUNCTION__, touchPos.x, touchPos.y);

			//タッチON
			gUiMng->setTouchOn(touchPos);

			return true;
		}
		else if (msg == WM_LBUTTONUP) {
			//タッチOFF
			gUiMng->setTouchOff();

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
				gUiMng->setDrag(dragPos);

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