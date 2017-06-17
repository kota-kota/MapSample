#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <new>
#include "WindowWGL.hpp"

//�R���X�g���N�^
window::WindowWGL::WindowWGL()
	: hInstance(nullptr)
{
	//�C���X�^���X�n���h���擾
	this->hInstance = ::GetModuleHandle(nullptr);
}

//�E�B���h�E�X�^�[�g
void window::WindowWGL::start()
{
	bool ret = true;

	//�R���\�[���E�B���h�E����
	FILE* fConsole = nullptr;
	::AllocConsole();
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//�E�B���h�E�E���X�o�^
	ret = this->registerWindowClass();
	if (!ret) {
		return;
	}

	//�E�C���h�E�쐬
	ret = this->createMainWindow();
	if (!ret) {
		return;
	}

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//�R���\�[���E�B���h�E�j��
	fclose(fConsole);
	//::FreeConsole();	//x64�ŗ�O����
}

//�E�B���h�E�N���X�o�^
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

//�E�B���h�E�쐬
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

		//�`��C���^�[�t�F�[�X����
		new(&this->drawWGL) draw::DrawWGL(hWnd);

		//UiMng����
		new(&this->uiMng) ui::UiMng(&this->drawWGL);

		// �E�B���h�E�\��
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	return result;
}

// �E�B���h�E�v���V�[�W��
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
		//�f�t�H���g����
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return (0L);
}

//WM_CREATE�C�x���g����
void window::WindowWGL::windowProcWMCreate(HWND hWnd, LPARAM lParam)
{
	//CreateWindow�̃p�����[�^����UiMng���擾
	LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
	ui::UiMng* uiMng = static_cast<ui::UiMng*>(lpCreateStruct->lpCreateParams);

	//hWnd��UiMng���֘A�t��
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(uiMng));

	printf("[%s] hWnd:0x%p UiMng:0x%p\n", __FUNCTION__, hWnd, uiMng);
}

//WM_DESTROY�C�x���g����
void window::WindowWGL::windowProcWMDestroy()
{
	::PostQuitMessage(0);
}

//WM_PAINT�C�x���g����
void window::WindowWGL::windowProcWMPaint(HWND hWnd)
{
	//hWnd�Ɋ֘A�t�����l����UiMng���擾
	ui::UiMng* uiMng = reinterpret_cast<ui::UiMng*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	printf("[%s] UiMng:0x%p\n", __FUNCTION__, uiMng);

	PAINTSTRUCT ps;
	::BeginPaint(hWnd, &ps);
	::EndPaint(hWnd, &ps);

	//�`��
	uiMng->draw();
}
