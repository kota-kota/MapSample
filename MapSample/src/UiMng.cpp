#include <cstdio>
#include "UiMng.hpp"


//�R���X�g���N�^
ui::UiMng::UiMng(draw::DrawIF* drawIF)
	: drawIF(drawIF)
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
}

//�`��
void ui::UiMng::draw()
{
	cmn::ColorU8 color = { 230,231, 232, 255 };
	this->drawIF->makeCurrent(true);
	this->drawIF->clear(color);
	this->drawIF->swapBuffers();
	this->drawIF->makeCurrent(false);
}
