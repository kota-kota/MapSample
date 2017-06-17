#pragma once
#include <Windows.h>
#include "DrawWGL.hpp"
#include "../UiMng.hpp"

namespace window
{
	class WindowWGL {
		//メンバ変数
		HINSTANCE		hInstance;
		draw::DrawWGL	drawWGL;
		ui::UiMng		uiMng;

	public:
		//コンストラクタ
		WindowWGL();
		//ウィンドウスタート
		void start();

	private:
		//ウィンドウクラス登録
		bool registerWindowClass();
		//ウィンドウ作成
		bool createMainWindow();
		// ウィンドウプロシージャ
		static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		//WM_CREATEイベント処理
		static void windowProcWMCreate(HWND hWnd, LPARAM lParam);
		//WM_DESTROYイベント処理
		static void windowProcWMDestroy();
		//WM_PAINTイベント処理
		static void windowProcWMPaint(HWND hWnd);
	};
}
