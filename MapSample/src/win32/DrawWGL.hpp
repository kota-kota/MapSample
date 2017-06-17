#pragma once
#include <Windows.h>
#include <gl/GL.h>

#include "Common.hpp"
#include "DrawIF.hpp"


namespace draw
{
	class DrawWGL : public DrawIF {
		HWND	hWnd;
		HDC		hDC;
		HGLRC	hGLRC;

	public:
		//�R���X�g���N�^
		DrawWGL(HWND hWnd = nullptr);
		//�f�X�g���N�^
		virtual ~DrawWGL();
		//�Z�b�g�A�b�v
		virtual void setup();
		//�`��J�����g
		virtual void makeCurrent(const bool current);
		//�`��X�V
		virtual void swapBuffers();
		//�N���A
		virtual void clear(const cmn::ColorU8& color);
		//�_�`��
		virtual void drawPoint(const cmn::PointI32& point);
	};
}
