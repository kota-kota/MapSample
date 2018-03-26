#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "DrawIF.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

namespace draw {

	class DrawGLX : public DrawIF {

		Display*	dpy_;
		Window		win_;
		GLXContext	ctx_;

	public:
		//コンストラクタ
		DrawGLX(Display* dpy, Window win, GLXContext ctx);
		//デストラクタ
		virtual ~DrawGLX();
		//セットアップ
		virtual void setup();
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const ColorUB& color);
		//ライン描画
		virtual void drawLines(const ColorUB& color, PointF* const points, const int32_t pointNum, const std::float32_t width);
	};
};


#endif //INCLUDED_DRAWWGL_HPP
