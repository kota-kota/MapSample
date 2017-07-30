#ifndef INCLUDED_COMMON
#define INCLUDED_COMMON

#include <cstdint>
#include <vector>

namespace cmn
{
	typedef char		Char_t;
	typedef int8_t		Int8_t;
	typedef int16_t		Int16_t;
	typedef int32_t		Int32_t;
	typedef int64_t		Int64_t;
	typedef uint8_t		UInt8_t;
	typedef uint16_t	UInt16_t;
	typedef uint32_t	UInt32_t;
	typedef uint64_t	UInt64_t;
	typedef float		Float32_t;
	typedef double		Float64_t;

	class CoordI16 {
	public:
		Int16_t		x;
		Int16_t		y;
		Int16_t		z;
	};

	class CoordI32 {
	public:
		Int32_t		x;
		Int32_t		y;
		Int32_t		z;
	};

	class CoordF64 {
	public:
		Float64_t	x;
		Float64_t	y;
		Float64_t	z;
	};

	class Color {
	public:
		UInt8_t		r;
		UInt8_t		g;
		UInt8_t		b;
		UInt8_t		a;
	};

	class Size {
	public:
		Int16_t		w;
		Int16_t		h;
	};

	class Area {
	public:
		Int32_t		xmin;
		Int32_t		ymin;
		Int32_t		xmax;
		Int32_t		ymax;
	};

	typedef std::vector<Color>		ColorVec;
	typedef std::vector<CoordI32>	CoordVec;
	typedef std::vector<UInt8_t>	Image;
	typedef std::vector<UInt8_t>	Binary;
}

#endif //INCLUDED_COMMON
