#ifndef INCLUDED_FONT_HPP
#define INCLUDED_FONT_HPP

#include "Std.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftsynth.h>


namespace image {

	class Character {
	public:
		//メンバ変数
		std::uint32_t	ax;		//advance.x
		std::uint32_t	ay;		//advance.y
		std::uint32_t	bw;		//bitmap.width
		std::uint32_t	bh;		//bitmap.rows
		std::uint32_t	bl;		//bitmap_left
		std::uint32_t	bt;		//bitmap_top
		std::uint8_t*	buffer;	//buffer

	public:
		//コンストラクタ
		Character();
		//デストラクタ
		~Character();
	};

	//フォントクラス
	class Font {
		//メンバ変数
		FT_Library	ftLibrary;
		FT_Face		ftFace;

	public:
		//コンストラクタ
		Font();
		//デストラクタ
		~Font();
		//ラスタライズ
		void rasterize(const wchar_t charCode, Character* const c);
	};
}

#endif //INCLUDED_FONT_HPP
