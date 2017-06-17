#include <Windows.h>
#include <cstdint>
#include <cstdio>

#include "DrawWGL.hpp"

namespace win32
{
	//グローバル変数
	static draw::DrawIF* drawIF = nullptr;

	//WM_CREATEイベント処理
	static void WindowProcWMCreate(HWND hWnd)
	{
		printf("%s hWnd:0x%p\n", __FUNCTION__, hWnd);

		//描画インターフェース生成
		drawIF = new draw::DrawWGL(hWnd);
	}

	//WM_DESTROYイベント処理
	static void WindowProcWMDestroy()
	{
		::PostQuitMessage(0);
	}

	//WM_PAINTイベント処理
	static void WindowProcWMPaint(HWND hWnd)
	{
		printf("%s\n", __FUNCTION__);
		PAINTSTRUCT ps;
		::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);
		if (drawIF != nullptr) {
			cmn::ColorU8 color = { 230,231, 232, 255 };
			drawIF->makeCurrent(true);
			drawIF->clear(color);
			drawIF->swapBuffers();
			drawIF->makeCurrent(false);
		}
	}


	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg) {
		case WM_CREATE:
			WindowProcWMCreate(hWnd);
			break;

		case WM_DESTROY:
			WindowProcWMDestroy();
			break;

		case WM_PAINT:
			WindowProcWMPaint(hWnd);
			break;

		default:
			//デフォルト処理
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return (0L);
	}

	//ウィンドウクラス登録
	bool RegisterWindowClass(HINSTANCE hInstance)
	{
		bool result = true;

		WNDCLASSEX wcex;
		memset(&wcex, 0, sizeof(WNDCLASSEX));

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = win32::WindowProc;
		wcex.lpszClassName = TEXT("MapSample");
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
		wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.hIconSm = ::LoadIcon(nullptr, IDI_APPLICATION);

		ATOM ret = ::RegisterClassEx(&wcex);
		if (ret == 0) {
			result = false;
		}

		return result;
	}

	//ウィンドウ作成
	bool CreateMainWindow(HINSTANCE hInstance)
	{
		bool result = true;

		HWND hWnd = ::CreateWindow(
			TEXT("MapSample"),
			TEXT("MapSample"),
			WS_OVERLAPPEDWINDOW,
			0,
			0,
			800,
			400,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);
		if (hWnd == nullptr) {
			result = false;
		}
		else {
			// ウィンドウ表示
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}

		return result;
	}

	//アプリケーションスタート
	void StartApplication()
	{
		bool ret = true;

		//コンソールウィンドウ生成
		FILE* fConsole = nullptr;
		::AllocConsole();
		freopen_s(&fConsole, "CONOUT$", "w", stdout);

		//インスタンスハンドル取得
		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		//ウィンドウウラス登録
		ret = RegisterWindowClass(hInstance);
		if (!ret) {
			return;
		}

		//ウインドウ作成
		ret = CreateMainWindow(hInstance);
		if (!ret) {
			return;
		}

		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//コンソールウィンドウ破棄
		fclose(fConsole);
		//::FreeConsole();	//x64で例外発生
	}
}
