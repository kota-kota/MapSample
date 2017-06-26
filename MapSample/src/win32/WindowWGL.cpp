#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <new>
#include "win32/WindowWGL.hpp"

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using window::WindowWGL;
using cmn::CoordI16;

//コンストラクタ
WindowWGL::WindowWGL()
	: hInstance(nullptr)
{
	//インスタンスハンドル取得
	this->hInstance = ::GetModuleHandle(nullptr);
}

//ウィンドウスタート
void WindowWGL::start()
{
	bool ret = true;

	//コンソールウィンドウ生成
	FILE* fConsole = nullptr;
	::AllocConsole();
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//ウィンドウウラス登録
	ret = this->registerWindowClass();
	if (!ret) {
		return;
	}

	//ウインドウ作成
	ret = this->createMainWindow();
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

//ウィンドウクラス登録
bool WindowWGL::registerWindowClass()
{
	bool result = true;

	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowWGL::windowProc;
	wcex.lpszClassName = TEXT("MapSample");
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = this->hInstance;
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
bool WindowWGL::createMainWindow()
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
		this->hInstance,
		static_cast<LPVOID>(&this->uiMng)
	);
	if (hWnd == nullptr) {
		result = false;
	}
	else {
		printf("[%s] DrawWGL:0x%p UiMng:0x%p\n", __FUNCTION__, &this->drawWGL, &this->uiMng);

		//描画インターフェース生成
		new(&this->drawWGL) draw::DrawWGL(hWnd);

		//UiMng生成
		new(&this->uiMng) ui::UiMng(&this->drawWGL);

		// ウィンドウ表示
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	return result;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WindowWGL::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool ret = true;

	switch (msg) {
	case WM_CREATE:
		ret = windowProcWMCreate(hWnd, lParam);
		break;

	case WM_DESTROY:
		ret = windowProcWMDestroy();
		break;

	case WM_PAINT:
		ret = windowProcWMPaint(hWnd);
		break;

	default:
		ret = windowProcUserOperation(hWnd, msg, wParam, lParam);
		if (!ret) {
			//デフォルト処理
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return (0L);
}

//WM_CREATEイベント処理
bool WindowWGL::windowProcWMCreate(HWND hWnd, LPARAM lParam)
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
bool WindowWGL::windowProcWMDestroy()
{
	::PostQuitMessage(0);

	return true;
}

//WM_PAINTイベント処理
bool WindowWGL::windowProcWMPaint(HWND hWnd)
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
bool WindowWGL::windowProcUserOperation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//hWndに関連付けた値からUiMngを取得
	ui::UiMng* uiMng = reinterpret_cast<ui::UiMng*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (msg == WM_LBUTTONDOWN) {
		CoordI16 screenPos = { 0, 0, 0 };
		screenPos.x = static_cast<int16_t>(LOWORD(lParam));
		screenPos.y = static_cast<int16_t>(HIWORD(lParam));
		printf("[%s] WM_LBUTTONDOWN:(%d, %d)\n", __FUNCTION__, screenPos.x, screenPos.y);

		//画面中心座標の更新
		uiMng->setScreenPosition(screenPos);

		RECT rect;
		::GetClientRect(hWnd, &rect);
		::InvalidateRect(hWnd, &rect, false);

		return true;
	}
	else if (msg == WM_MOUSEMOVE) {
		if (wParam & MK_LBUTTON) {
			CoordI16 screenPos = { 0, 0, 0 };
			screenPos.x = static_cast<int16_t>(LOWORD(lParam));
			screenPos.y = static_cast<int16_t>(HIWORD(lParam));
			printf("[%s] WM_MOUSEMOVE:(%d, %d)\n", __FUNCTION__, screenPos.x, screenPos.y);

			//画面中心座標の更新
			uiMng->setScreenPosition(screenPos);

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
