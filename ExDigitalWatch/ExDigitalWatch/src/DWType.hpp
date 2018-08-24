#ifndef INCLUDED_DWTYPE_HPP
#define INCLUDED_DWTYPE_HPP

#include <cstdint>

namespace std {
	typedef char char8_t;
	typedef float float32_t;
	typedef double float64_t;
}

namespace dw {

	//サイズ
	struct DWSize {
		std::int32_t	width_;
		std::int32_t	height_;
	};

	//エリア
	struct DWArea {
		std::int32_t	xmin_;
		std::int32_t	ymin_;
		std::int32_t	xmax_;
		std::int32_t	ymax_;
	};

	//座標
	struct DWCoord {
		std::int32_t	x_;
		std::int32_t	y_;
	};

	//色
	struct DWColor {
		std::uint8_t	r_;
		std::uint8_t	g_;
		std::uint8_t	b_;
		std::uint8_t	a_;
	};

	//テキスト
	struct DWText {
		std::int32_t	num_;
		std::uint16_t	text_[32];
	};

	//時刻
	struct DWTime {
		std::int32_t	h_;
		std::int32_t	m_;
		std::int32_t	s_;
		std::int32_t	strNum_;
		std::char8_t	str_[32];
	};
};


#endif //INCLUDED_DWTYPE_HPP
