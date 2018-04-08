#include "WindowIF.hpp"
#include "WindowX.hpp"

int main()
{
	window::WindowIF* windowIF = new window::WindowX();
	windowIF->create();
	return windowIF->start();
}
