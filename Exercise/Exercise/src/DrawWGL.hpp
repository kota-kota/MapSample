#ifndef INCLUDED_DRAWWGL_HPP
#define INCLUDED_DRAWWGL_HPP

#include "Type.hpp"
#include <Windows.h>


namespace draw {

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
	// 画像デコードI/Fクラス
	//
	//----------------------------------------------------------

	class ImageDecorder {
	public:
		//デストラクタ
		virtual ~ImageDecorder() {]
		//デコードデータを取得
		virtual std::uint8_t* getDecodeData(std::int32_t* const decodeSize, std::int32_t* const width, std::int32_t* const height) = 0;
		//RGBA8888画像へデコード
		virtual void decodeRgba8888() = 0;
	};


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
		void clear(const draw::ColorUB& color);
		//イメージ描画
		//void drawImage(const draw::CoordI& coord, const fw::Image& image);
		//文字描画
		//void drawString(const std::CoordI& coord, const wchar_t* const str);
	};
}

#endif //INCLUDED_DRAWWGL_HPP
