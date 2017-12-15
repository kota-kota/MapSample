#ifndef INCLUDED_FONT_HPP
#define INCLUDED_FONT_HPP

#include "Std.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftsynth.h>


namespace image {

	class Character {
	public:
		//�����o�ϐ�
		std::uint32_t	ax;		//advance.x
		std::uint32_t	ay;		//advance.y
		std::uint32_t	bw;		//bitmap.width
		std::uint32_t	bh;		//bitmap.rows
		std::uint32_t	bl;		//bitmap_left
		std::uint32_t	bt;		//bitmap_top
		std::uint8_t*	buffer;	//buffer

	public:
		//�R���X�g���N�^
		Character();
		//�f�X�g���N�^
		~Character();
	};

	//�t�H���g�N���X
	class Font {
		//�����o�ϐ�
		FT_Library	ftLibrary;
		FT_Face		ftFace;

	public:
		//�R���X�g���N�^
		Font();
		//�f�X�g���N�^
		~Font();
		//���X�^���C�Y
		void rasterize(const wchar_t charCode, Character* const c);
	};
}

#endif //INCLUDED_FONT_HPP
