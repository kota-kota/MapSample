#include "WindowCreaterX.hpp"

int main()
{
	window::WindowCreater* windowCreater = new window::WindowCreaterX();
	int rc = windowCreater->create();
	if(rc == 0) {
		rc = windowCreater->start();
	}
	delete windowCreater;
	return rc;
}
