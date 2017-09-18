#ifndef INCLUDED_WINDOWWGL_HPP
#define INCLUDED_WINDOWWGL_HPP

#include "FwStd.hpp"
#include "graphics/win32/DrawWGL.hpp"
#include "UiMng.hpp"
#include <Windows.h>
#include <new>

namespace window
{
	//----------------------------------------------------------
	//
	// WGLウィンドウクラス
	//
	//----------------------------------------------------------

	class WindowWGL {
		//メンバ変数
		HINSTANCE		hInstance_;
		fw::DrawWGL		drawWGL_;
		ui::UiMng		uiMng_;

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
		static bool windowProcWMCreate(HWND hWnd, LPARAM lParam);
		//WM_DESTROYイベント処理
		static bool windowProcWMDestroy();
		//WM_PAINTイベント処理
		static bool windowProcWMPaint(HWND hWnd);
		//ユーザ操作イベント処理
		static bool windowProcUserOperation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}

#endif //INCLUDED_WINDOWWGL_HPP
