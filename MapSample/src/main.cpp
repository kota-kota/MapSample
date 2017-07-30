#include "Common.hpp"
#include "ui/WindowWGL.hpp"

//using指定
using cmn::Int32_t;
using window::WindowWGL;

//メイン処理
Int32_t main()
{
	//ウィンドウ作成・スタート
	WindowWGL window;
	window.start();

	return 0;
}