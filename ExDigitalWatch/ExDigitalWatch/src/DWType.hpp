#ifndef INCLUDED_DWTYPE_HPP
#define INCLUDED_DWTYPE_HPP

#include <cstdint>

namespace {
	typedef float float32_t;
	typedef double float64_t;
}

namespace dw {

	//サイズ
	struct DWSize {
		int		width_;
		int		height_;
	};

	//エリア
	struct DWArea {
		int		xmin_;
		int		ymin_;
		int		xmax_;
		int		ymax_;
	};

	//座標
	struct DWCoord {
		int		x_;
		int		y_;
	};

	//色
	struct DWColor {
		float	r_;
		float	g_;
		float	b_;
		float	a_;
	};

	//テキスト
	struct DWText {
		int		num_;
		wchar_t	text_[256];
	};
};


#endif //INCLUDED_DWTYPE_HPP
