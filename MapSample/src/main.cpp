#include "Common.hpp"
#include "ui/WindowWGL.hpp"

//using指定
using std::int32_t;
using window::WindowWGL;

//メイン処理
int32_t main()
{
	//ウィンドウ作成・スタート
	WindowWGL window;
	window.start();

	return 0;
}