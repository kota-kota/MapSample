#pragma once
#include <Windows.h>
#include "DrawWGL.hpp"
#include "../UiMng.hpp"

namespace window
{
	class WindowWGL {
		//�����o�ϐ�
		HINSTANCE		hInstance;
		draw::DrawWGL	drawWGL;
		ui::UiMng		uiMng;

	public:
		//�R���X�g���N�^
		WindowWGL();
		//�E�B���h�E�X�^�[�g
		void start();

	private:
		//�E�B���h�E�N���X�o�^
		bool registerWindowClass();
		//�E�B���h�E�쐬
		bool createMainWindow();
		// �E�B���h�E�v���V�[�W��
		static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		//WM_CREATE�C�x���g����
		static void windowProcWMCreate(HWND hWnd, LPARAM lParam);
		//WM_DESTROY�C�x���g����
		static void windowProcWMDestroy();
		//WM_PAINT�C�x���g����
		static void windowProcWMPaint(HWND hWnd);
	};
}
