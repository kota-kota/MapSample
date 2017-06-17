#pragma once
#include <cstdint>

namespace cmn
{
	class ColorU8 {
	public:
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
		std::uint8_t	a;
	};

	class PointI16 {
	public:
		std::int16_t	x;
		std::int16_t	y;
		std::int16_t	z;
	};

	class PointI32 {
	public:
		std::int32_t	x;
		std::int32_t	y;
		std::int32_t	z;
	};

	class PointD {
	public:
		double	x;
		double	y;
		double	z;
	};
}
