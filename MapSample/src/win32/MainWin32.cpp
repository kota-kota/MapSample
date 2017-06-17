#include <Windows.h>
#include <cstdint>
#include <cstdio>

#include "DrawWGL.hpp"

namespace win32
{
	//�O���[�o���ϐ�
	static draw::DrawIF* drawIF = nullptr;

	//WM_CREATE�C�x���g����
	static void WindowProcWMCreate(HWND hWnd)
	{
		printf("%s hWnd:0x%p\n", __FUNCTION__, hWnd);

		//�`��C���^�[�t�F�[�X����
		drawIF = new draw::DrawWGL(hWnd);
	}

	//WM_DESTROY�C�x���g����
	static void WindowProcWMDestroy()
	{
		::PostQuitMessage(0);
	}

	//WM_PAINT�C�x���g����
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


	// �E�B���h�E�v���V�[�W��
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
			//�f�t�H���g����
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return (0L);
	}

	//�E�B���h�E�N���X�o�^
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

	//�E�B���h�E�쐬
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
			// �E�B���h�E�\��
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}

		return result;
	}

	//�A�v���P�[�V�����X�^�[�g
	void StartApplication()
	{
		bool ret = true;

		//�R���\�[���E�B���h�E����
		FILE* fConsole = nullptr;
		::AllocConsole();
		freopen_s(&fConsole, "CONOUT$", "w", stdout);

		//�C���X�^���X�n���h���擾
		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		//�E�B���h�E�E���X�o�^
		ret = RegisterWindowClass(hInstance);
		if (!ret) {
			return;
		}

		//�E�C���h�E�쐬
		ret = CreateMainWindow(hInstance);
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
}
