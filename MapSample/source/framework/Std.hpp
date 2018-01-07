#ifndef INCLUDED_STD_HPP
#define INCLUDED_STD_HPP

#include <cstdint>

#define DRAWIF_WGL
//#define DRAWIF_WEGL

namespace std {

	//----------------------------------------------------------
	//
	// 共通型定義
	//
	//----------------------------------------------------------

	using char_t = char;
	using float_t = float;
	using double_t = double;

	//----------------------------------------------------------
	//
	// 共通列挙体定義
	//
	//----------------------------------------------------------

	//OFFON
	enum EN_OffOn { OFF, ON, };


	//----------------------------------------------------------
	//
	// 共通構造体定義
	//
	//----------------------------------------------------------

	//座標
	template <typename T> struct Coord {
		T	x;
		T	y;
		T	z;
	};
	using CoordI = Coord<int32_t>;
	using CoordF = Coord<float_t>;
	using CoordD = Coord<double_t>;

	//UV座標
	template <typename T> struct CoordTex {
		T	u;
		T	v;
	};
	using CoordTexI = CoordTex<int32_t>;
	using CoordTexF = CoordTex<float_t>;
	using CoordTexD = CoordTex<double_t>;

	//色
	template <typename T> struct Color {
		T	r;
		T	g;
		T	b;
		T	a;
	};
	using ColorUB = Color<uint8_t>;
	using ColorF = Color<float_t>;

	//エリア
	template <typename T> struct Area {
		T	xmin;
		T	ymin;
		T	xmax;
		T	ymax;
	};
	using AreaI = Area<int32_t>;
	using AreaF = Area<float_t>;

	//位置
	struct Position {
		int32_t		x;
		int32_t		y;
	};

	//幅高さ
	struct WH {
		int32_t		width;
		int32_t		height;
	};


	//----------------------------------------------------------
	//
	// リソース定義
	//
	//----------------------------------------------------------

	//地図中心座標
	static const std::Position D_MAP_POSITION = { 63230028, 16092608 };

	//dataフォルダのパス
	static const char_t* const D_DATA_PATH = "C:/cygwin64/home/Kyohei/program/data";
}

#endif //INCLUDED_FWSTD_HPP
