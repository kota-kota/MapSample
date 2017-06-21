#include <cstdint>
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

//メイン処理
int32_t main()
{
	//ウィンドウ作成・スタート
	WindowWGL window;
	window.start();

	return 0;
}