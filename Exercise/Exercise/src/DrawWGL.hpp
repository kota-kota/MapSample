#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "Type.hpp"
#include <Windows.h>
#include <string>


namespace ui {

	//座標
	template <typename T> struct Coord {
		T	x;
		T	y;
		T	z;
	};
	using CoordI = Coord<std::int32_t>;
	using CoordF = Coord<std::float32_t>;
	using CoordD = Coord<std::float64_t>;

	//UV座標
	template <typename T> struct CoordTex {
		T	u;
		T	v;
	};
	using CoordTexI = CoordTex<std::int32_t>;
	using CoordTexF = CoordTex<std::float32_t>;
	using CoordTexD = CoordTex<std::float64_t>;

	//エリア
	template <typename T> struct Area {
		T	xmin;
		T	ymin;
		T	xmax;
		T	ymax;
	};
	using AreaI = Area<std::int32_t>;
	using AreaF = Area<std::float32_t>;

	//色
	template <typename T> struct Color {
		T	r;
		T	g;
		T	b;
		T	a;
	};
	using ColorUB = Color<std::uint8_t>;
	using ColorF = Color<std::float32_t>;


	//----------------------------------------------------------
	//
	// WGL描画クラス
	//
	//----------------------------------------------------------

	class DrawWGL {
		HWND			hWnd_;
		HDC				hDC_;
		HGLRC			hGLRC_;

	public:
		//コンストラクタ
		DrawWGL(const HWND hWnd);
		//デストラクタ
		~DrawWGL();
		//作成
		void create();
		//セットアップ
		void setup();
		//描画カレント
		void makeCurrent(const bool current);
		//描画更新
		void swapBuffers();
		//クリア
		void clear(const ui::ColorUB& color);
		//イメージ描画
		void drawImage(const ui::CoordI& coord, const uint8_t* const image, const int32_t width, const int32_t height);
		//文字描画
		void drawString(const ui::CoordI& coord, const std::wstring& str);
	};
}

#endif //INCLUDED_DRAWWGL_HPP
