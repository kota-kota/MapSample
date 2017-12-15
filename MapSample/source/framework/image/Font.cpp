#include "Font.hpp"
#include <string>

namespace {
	//フォントファイル
	const std::string fontFile = "font/ipagp.ttf";
}


//コンストラクタ
image::Character::Character() :
	ax(0), ay(0), bw(0), bh(0), bl(0), bt(0), buffer(nullptr)
{
}

//デストラクタ
image::Character::~Character()
{
	if (this->buffer != nullptr) {
		delete[] this->buffer;
	}
}


//コンストラクタ
image::Font::Font()
	: ftLibrary(nullptr), ftFace(nullptr)
{
	//dataフォルダパス
	const std::string dataPath = std::D_DATA_PATH;

	//フォントファイルパス
	const std::string fontFilePath = dataPath + "/" + fontFile;

	//FreeType初期化
	FT_Init_FreeType(&this->ftLibrary);
	FT_New_Face(this->ftLibrary, fontFilePath.c_str(), 0, &this->ftFace);
}

//デストラクタ
image::Font::~Font()
{
	FT_Done_Face(this->ftFace);
	FT_Done_FreeType(this->ftLibrary);
}

//ラスタライズ
void image::Font::rasterize(const wchar_t charCode, Character* const c)
{
	FT_Face face = this->ftFace;

	FT_Set_Char_Size(face, 0, 24*24, 300, 300);
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
