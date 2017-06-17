#pragma once
#include "Common.hpp"

namespace draw
{
	class DrawIF {
	public:
		//�f�X�g���N�^
		virtual ~DrawIF() {}
		//�Z�b�g�A�b�v
		virtual void setup() = 0;
		//�`��J�����g
		virtual void makeCurrent(const bool current) = 0;
		//�`��X�V
		virtual void swapBuffers() = 0;
		//�N���A
		virtual void clear(const cmn::ColorU8& color) = 0;
		//�_�`��
		virtual void drawPoint(const cmn::PointI32& point) = 0;
	};
}