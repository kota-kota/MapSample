#include <Windows.h>
#include <cstdint>
#include <vector>

#include "DrawWGL.hpp"

#pragma comment(lib, "opengl32.lib")


//�R���X�g���N�^
draw::DrawWGL::DrawWGL(HWND hWnd)
	: hWnd(hWnd), hDC(nullptr), hGLRC(nullptr)
{
	//�f�o�C�X�R���e�L�X�g�n���h�����擾
	this->hDC = ::GetDC(this->hWnd);

	//�s�N�Z���t�H�[�}�b�g
	const PIXELFORMATDESCRIPTOR pFormat = {
		sizeof(PIXELFORMATDESCRIPTOR),	//nSize
		1,		//nVersion
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//dwFlags
		PFD_TYPE_RGBA,	//iPixelType
		32,		//cColorBits
		0,		//cRedBits
		0,		//cRedShift
		0,		//cGreenBits
		0,		//cGreenShift
		0,		//cBlueBits
		0,		//cBlueShift
		0,		//cAlphaBits
		0,		//cAlphaShift
		0,		//cAccumBits
		0,		//cAccumRedBits
		0,		//cAccumGreenBits
		0,		//cAccumBlueBits
		0,		//cAccumAlphaBits
		0,		//cDepthBits
		0,		//cStencilBits
		0,		//cAuxBuffers
		0,		//iLayerType
		0,		//bReserved
		0,		//dwLayerMask
		0,		//dwVisibleMask
		0		//dwDamageMask
	};

	//�s�N�Z���t�H�[�}�b�g��I��
	std::int32_t format = ::ChoosePixelFormat(this->hDC, &pFormat);
	::SetPixelFormat(this->hDC, format, &pFormat);

	//�`��R���e�L�X�g�n���h�����쐬
	this->hGLRC = ::wglCreateContext(this->hDC);
}

//�f�X�g���N�^
draw::DrawWGL::~DrawWGL()
{
	//�`��R���e�L�X�g�n���h����j��
	::wglDeleteContext(this->hGLRC);

	//�f�o�C�X�R���e�L�X�g��j��
	::ReleaseDC(this->hWnd, this->hDC);
}

//�`��Z�b�g�A�b�v
void draw::DrawWGL::setup()
{
	RECT rect;
	::GetClientRect(this->hWnd, &rect);
	std::int32_t width = rect.right - rect.left;
	std::int32_t height = rect.bottom - rect.top;

	//�r���[�|�[�g�ݒ�
	glViewport(0, 0, width, height);
}

//�`��J�����g
void draw::DrawWGL::makeCurrent(const bool current)
{
	if (current) {
		//�J�����g�ݒ�
		::wglMakeCurrent(this->hDC, this->hGLRC);
	}
	else {
		//�J�����g����
		::wglMakeCurrent(this->hDC, nullptr);
	}
}

//�`��X�V
void draw::DrawWGL::swapBuffers()
{
	::SwapBuffers(this->hDC);
}

//�N���A
void draw::DrawWGL::clear(const cmn::ColorU8& color)
{
	GLclampf r = static_cast<GLclampf>(color.r) / 255.0f;
	GLclampf g = static_cast<GLclampf>(color.g) / 255.0f;
	GLclampf b = static_cast<GLclampf>(color.b) / 255.0f;
	GLclampf a = static_cast<GLclampf>(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

//�_�`��
void draw::DrawWGL::drawPoint(const cmn::PointI32& point)
{
}
