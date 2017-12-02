#ifndef INCLUDED_STD_HPP
#define INCLUDED_STD_HPP

#include <cstdint>
#include <memory>

namespace std {

	using char_t = char;
	using float_t = float;
	using double_t = double;


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
}

#endif //INCLUDED_FWSTD_HPP
