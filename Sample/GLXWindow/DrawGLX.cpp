#include "DrawGLX.hpp"

namespace draw {
	//コンストラクタ
	DrawGLX::DrawGLX(Display* dpy, Window win, GLXContext ctx) :
		dpy_(dpy), win_(win), ctx_(ctx)
	{
	}

	//デストラクタ
	DrawGLX::~DrawGLX()
	{
	}

	//セットアップ
	void DrawGLX::setup()
	{
		XWindowAttributes wAttr;
		XGetWindowAttributes(this->dpy_, this->win_, &wAttr);
		glViewport(0, 0, wAttr.width, wAttr.height);

		glMatrixMode(GL_PROJECTION_MATRIX);
		glOrtho(0, wAttr.width, 0, wAttr.height, -1, 1);
	}

	//描画カレント
	void DrawGLX::makeCurrent(const bool current)
	{
		if (current) {
			glXMakeCurrent(this->dpy_, this->win_, this->ctx_);
		}
		else {
			glXMakeCurrent(this->dpy_, None, nullptr);
		}
	}

	//描画更新
	void DrawGLX::swapBuffers()
	{
		glXSwapBuffers(this->dpy_, this->win_);
	}

	//クリア
	void DrawGLX::clear(const ColorUB& color)
	{
		std::float32_t r = std::float32_t(color.r_) / 255.0F;
		std::float32_t g = std::float32_t(color.g_) / 255.0F;
		std::float32_t b = std::float32_t(color.b_) / 255.0F;
		std::float32_t a = std::float32_t(color.a_) / 255.0F;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//ライン描画
	void DrawGLX::drawLines(const ColorUB& color, PointF* const points, const int32_t pointNum, const std::float32_t width)
	{
		//頂点配列の転送開始
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, points);

		//ライン幅
		glLineWidth(width);

		//ライン色
		glColor4ub(color.r_, color.g_, color.b_, color.a_);

		//ライン描画
		glDrawArrays(GL_LINE_STRIP, 0, pointNum);

		//頂点配列の転送終了
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}
