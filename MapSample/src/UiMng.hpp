#pragma once
#include "DrawIF.hpp"

namespace ui
{
	class UiMng {
		//�����o�ϐ�
		draw::DrawIF*	drawIF;

	public:
		//�R���X�g���N�^
		UiMng(draw::DrawIF* drawIF = nullptr);
		//�`��
		void draw();
	};
}
