#include "Std.hpp"
#include "ui/Window.hpp"
#include <string>


//メイン処理
std::int32_t main()
{
	//ウィンドウクラス作成
	ui::WindowFactory factory;
	ui::Window* window = factory.create(ui::EN_WindowType::WIN32_WGL_GL1);

	//ウィンドウ作成
	std::string name = "MapSample";
	std::Position pos = { 0, 0 };
	std::WH wh = { 800, 400 };
	window->create(name, pos, wh);

	//ウィンドウ開始
	window->start();

	return 0;
}