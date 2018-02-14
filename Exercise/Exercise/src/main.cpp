#include "Type.hpp"
#include "DigitalWatch.hpp"
#include <Windows.h>
#include <cstdio>
#include <string>


namespace {

	//----------------------------------------------------------
	//
	// グローバル変数
	//
	//----------------------------------------------------------

	//デジタル時計オブジェクト
	static ui::DigitalWatch* gDigitalWatch = nullptr;

	//----------------------------------------------------------
	//
	// 定数定義
	//
	//----------------------------------------------------------

	//ウインドウ関連
	static const std::string winName = "Exercise";
	static const std::int32_t winPosX = 0;
	static const std::int32_t winPosY = 0;
	static const std::int32_t winWidth = 800;
	static const std::int32_t winHeight = 500;
}

//関数
namespace {

	//WM_CREATEイベント処理
	static void winproc_create(const HWND hWnd)
	{
		//デジタル時計オブジェクト生成
		gDigitalWatch = new ui::DigitalWatch(hWnd);

		printf("[WM_CREATE] hWnd:0x%p gDigitalWatch:0x%p\n", hWnd, gDigitalWatch);
	}

	//WM_DESTROYイベント処理
	static void winproc_destroy()
	{
		if (gDigitalWatch != nullptr) {
			delete gDigitalWatch;
		}

		::PostQuitMessage(0);
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
			gDigitalWatch->draw();
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
	wcex.lpszClassName = TEXT(winName.c_str());
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
		TEXT(winName.c_str()),
		TEXT(winName.c_str()),
		WS_OVERLAPPEDWINDOW,
		winPosX,
		winPosY,
		winWidth,
		winHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	if (hWnd == nullptr) {
		printf("!!! Failed to CreateWindow\n");
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