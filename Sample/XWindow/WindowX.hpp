#ifndef INCLUDED_WINDOWX_HPP
#define INCLUDED_WINDOWX_HPP

#include "WindowIF.hpp"
#include "DrawIF.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

namespace window {

	class WindowX : public WindowIF {

		Display*		dpy_;
		Window			win_;
		GLXContext		ctx_;
		draw::DrawIF*	drawIF_;

	public:
		//コンストラクタ
		WindowX();
		//デストラクタ
		virtual ~WindowX();
		//ウィンドウ作成
		virtual int create();
		//ウィンドウ開始
		virtual int start();
	};
}

#endif //INCLUDED_WINDOWX_HPP
