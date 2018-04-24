#include "Font.hpp"
#include <string>

namespace {
	//フォントファイル
	const std::string fontFilePath = "./data/font/ipagp.ttf";
}

namespace draw {
	//コンストラクタ
	Character::Character() :
		ax(0), ay(0), bw(0), bh(0), bl(0), bt(0), buffer(nullptr)
	{
	}

	//デストラクタ
	Character::~Character()
	{
		if (this->buffer != nullptr) {
			delete[] this->buffer;
		}
	}


	//コンストラクタ
	Font::Font()
		: ftLibrary(nullptr), ftFace(nullptr)
	{
		//FreeType初期化
		FT_Init_FreeType(&this->ftLibrary);
		FT_New_Face(this->ftLibrary, fontFilePath.c_str(), 0, &this->ftFace);
	}

	//デストラクタ
	Font::~Font()
	{
		FT_Done_Face(this->ftFace);
		FT_Done_FreeType(this->ftLibrary);
	}

	//ラスタライズ
	void Font::rasterize(const std::char8_t charCode, Character* const c)
	{
		FT_Face face = this->ftFace;

		FT_Set_Char_Size(face, 20 * 64, 20 * 64, 96, 96);
		//FT_Set_Pixel_Sizes(face, 32, 32);
		FT_UInt glyphIndex = FT_Get_Char_Index(face, charCode);
		FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);

		//装飾
		//FT_GlyphSlot_Embolden(face->glyph);	//太字
		//FT_GlyphSlot_Oblique(face->glyph);	//斜体

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		c->ax = face->glyph->advance.x;
		c->ay = face->glyph->advance.y;
		c->bw = face->glyph->bitmap.width;
		c->bh = face->glyph->bitmap.rows;
		c->bl = face->glyph->bitmap_left;
		c->bt = face->glyph->bitmap_top;
		std::int32_t bufSize = c->bw * c->bh;
		c->buffer = new std::uint8_t[bufSize];
		memcpy_s(c->buffer, bufSize, face->glyph->bitmap.buffer, bufSize);
	}

}