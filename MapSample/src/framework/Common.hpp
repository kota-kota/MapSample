#ifndef INCLUDED_COMMON
#define INCLUDED_COMMON

#include <cstdint>

namespace std
{
	typedef char	char_t;
	typedef float	float32_t;
	typedef double	float64_t;
}

namespace cmn
{
	class CoordI16 {
	public:
		std::int16_t	x;
		std::int16_t	y;
		std::int16_t	z;
	};

	class CoordI32 {
	public:
		std::int32_t	x;
		std::int32_t	y;
		std::int32_t	z;
	};

	class CoordF64 {
	public:
		std::float64_t	x;
		std::float64_t	y;
		std::float64_t	z;
	};

	class Color {
	public:
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
		std::uint8_t	a;
	};

	class Size {
	public:
		std::int16_t	w;
		std::int16_t	h;
	};

	class Area {
	public:
		std::int32_t	xmin;
		std::int32_t	ymin;
		std::int32_t	xmax;
		std::int32_t	ymax;
	};
}

#endif //INCLUDED_COMMON
