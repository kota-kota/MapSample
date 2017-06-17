#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <new>
#include "WindowWGL.hpp"

//コンストラクタ
window::WindowWGL::WindowWGL()
	: hInstance(nullptr)
{
	//インスタンスハンドル取得
	this->hInstance = ::GetModuleHandle(nullptr);
}

//ウィンドウスタート
void window::WindowWGL::start()
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
bool window::WindowWGL::registerWindowClass()
{
	bool result = true;

	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = window::WindowWGL::windowProc;
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
bool window::WindowWGL::createMainWindow()
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
LRESULT CALLBACK window::WindowWGL::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		windowProcWMCreate(hWnd, lParam);
		break;

	case WM_DESTROY:
		windowProcWMDestroy();
		break;

	case WM_PAINT:
		windowProcWMPaint(hWnd);
		break;

	default:
		//デフォルト処理
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return (0L);
}

//WM_CREATEイベント処理
void window::WindowWGL::windowProcWMCreate(HWND hWnd, LPARAM lParam)
{
	//CreateWindowのパラメータからUiMngを取得
	LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
	ui::UiMng* uiMng = static_cast<ui::UiMng*>(lpCreateStruct->lpCreateParams);

	//hWndにUiMngを関連付け
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(uiMng));

	printf("[%s] hWnd:0x%p UiMng:0x%p\n", __FUNCTION__, hWnd, uiMng);
}

//WM_DESTROYイベント処理
void window::WindowWGL::windowProcWMDestroy()
{
	::PostQuitMessage(0);
}

//WM_PAINTイベント処理
void window::WindowWGL::windowProcWMPaint(HWND hWnd)
{
	//hWndに関連付けた値からUiMngを取得
	ui::UiMng* uiMng = reinterpret_cast<ui::UiMng*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	printf("[%s] UiMng:0x%p\n", __FUNCTION__, uiMng);

	PAINTSTRUCT ps;
	::BeginPaint(hWnd, &ps);
	::EndPaint(hWnd, &ps);

	//描画
	uiMng->draw();
}
