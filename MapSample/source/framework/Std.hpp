#ifndef INCLUDED_STD_HPP
#define INCLUDED_STD_HPP

#include <cstdint>
#include <memory>

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

	enum EN_OffOn { OFF, ON, };


	//----------------------------------------------------------
	//
	// 共通構造体定義
	//
	//----------------------------------------------------------

	struct CoordI {
		int32_t		x;
		int32_t		y;
		int32_t		z;
	};

	struct CoordD {
		double_t	x;
		double_t	y;
		double_t	z;
	};

	struct CoordTex {
		double_t	u;
		double_t	v;
	};

	struct Color {
		uint8_t		r;
		uint8_t		g;
		uint8_t		b;
		uint8_t		a;
	};

	struct Position {
		int32_t		x;
		int32_t		y;
	};

	struct WH {
		int32_t		width;
		int32_t		height;
	};

	struct Area {
		int32_t		xmin;
		int32_t		ymin;
		int32_t		xmax;
		int32_t		ymax;
	};


	//----------------------------------------------------------
	//
	// リソース定義
	//
	//----------------------------------------------------------

	//地図中心座標
	static const std::CoordI D_MAP_POSITION = { 63230028, 16092608, 0 };

	//dataフォルダのパス
	static const char_t* const D_DATA_PATH = "C:/cygwin64/home/Kyohei/program/data";
}

#endif //INCLUDED_FWSTD_HPP
