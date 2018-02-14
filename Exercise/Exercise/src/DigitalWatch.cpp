#include "DigitalWatch.hpp"
#include "DrawWGL.hpp"
#include "Image.hpp"
#include <fstream>
#include <string>

//FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

namespace {
	const std::string topPath = "./image";

	//デジタル時計画像データ
	struct DigitalWatchImage {
		std::uint8_t		id_;
		ui::EN_ImageFormat	format_;
		std::string			filePath_;
	};

	//デジタル時計画像データテーブル
	static const DigitalWatchImage tblDigitalWatchImage[] = {
		{ 0x00,	ui::EN_ImageFormat::PNG,	"0_zero.png" },
		{ 0x01,	ui::EN_ImageFormat::PNG,	"1_one.png" },
		{ 0x02,	ui::EN_ImageFormat::PNG,	"2_two.png" },
		{ 0x03,	ui::EN_ImageFormat::PNG,	"3_three.png" },
		{ 0x04,	ui::EN_ImageFormat::PNG,	"4_four.png" },
		{ 0x05,	ui::EN_ImageFormat::PNG,	"5_five.png" },
		{ 0x06,	ui::EN_ImageFormat::PNG,	"6_six.png" },
		{ 0x07,	ui::EN_ImageFormat::PNG,	"7_seven.png" },
		{ 0x08,	ui::EN_ImageFormat::PNG,	"8_eight.png" },
		{ 0x09,	ui::EN_ImageFormat::PNG,	"9_nine.png" },
		{ 0xf0,	ui::EN_ImageFormat::PNG,	"sym_colon.png" },
	};
	static const int32_t tblDigitalWatchImageNum = sizeof(tblDigitalWatchImage) / sizeof(tblDigitalWatchImage[0]);

#define WIDTH   320
#define HEIGHT  240
	unsigned char txtimage[HEIGHT][WIDTH];
	void draw_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y)
	{
		FT_Int  i, j, p, q;
		FT_Int  x_max = x + bitmap->width;
		FT_Int  y_max = y + bitmap->rows;

		for (i = x, p = 0; i < x_max; i++, p++)
		{
			for (j = y, q = 0; j < y_max; j++, q++)
			{
				if (i < 0 || j < 0 ||
					i >= WIDTH || j >= HEIGHT)
					continue;

				txtimage[j][i] |= bitmap->buffer[q * bitmap->width + p];
			}
		}
	}
	void test_freetype()
	{
		FT_Library    library;
		FT_Face       face;

		FT_GlyphSlot  slot;
		FT_Matrix     matrix;                 /* transformation matrix */
		FT_Vector     pen;                    /* untransformed origin  */
		FT_Error      error;

		char*         filename;
		wchar_t*         text;

		double        angle;
		int           target_height;
		int           n, num_chars;


		filename = "ipagp.ttf";                           /* first argument     */
		text = L"opqrstu";                           /* second argument    */
		num_chars = wcslen(text);
		target_height = HEIGHT;

		error = FT_Init_FreeType(&library);              /* initialize library */
														 /* error handling omitted */

		error = FT_New_Face(library, filename, 0, &face);/* create face object */
														 /* error handling omitted */

														 /* use 50pt at 100dpi */
		//error = FT_Set_Char_Size(face, 20 * 64, 0, 96, 0);                /* set character size */
									/* error handling omitted */
		FT_Set_Pixel_Sizes(face, 20, 20);
		slot = face->glyph;

		/* the pen position in 26.6 cartesian space coordinates; */
		/* start at (300,200) relative to the upper left corner  */
		//pen.x = 300 * 64;
		//pen.y = (target_height - 200) * 64;
		pen.x = 0;
		pen.y = 0;

		for (n = 0; n < num_chars; n++)
		{
			/* set transformation */
			FT_Set_Transform(face, nullptr, &pen);

			/* load glyph image into the slot (erase previous one) */
			error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
			if (error)
				continue;                 /* ignore errors */

										  /* now, draw to our target surface (convert position) */
			draw_bitmap(&slot->bitmap, slot->bitmap_left, target_height - slot->bitmap_top);
			//draw_bitmap(&slot->bitmap, slot->bitmap_left, slot->bitmap_top);

			/* increment pen position */
			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
		}

		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}
};


namespace ui {

	//----------------------------------------------------------
	//
	// デジタル時計クラス実装
	//
	//----------------------------------------------------------

	//コンストラクタ
	DigitalWatch::DigitalWatch(const HWND hWnd) :
		hWnd_(hWnd), drawIF_(nullptr)
	{
		this->drawIF_ = new ui::DrawWGL(hWnd);
		this->drawIF_->create();
	}

	//デストラクタ
	DigitalWatch::~DigitalWatch()
	{
		delete this->drawIF_;
	}

	//描画
	void DigitalWatch::draw()
	{
		std::uint8_t* data = nullptr;
		std::int32_t dataSize = 0;

		//バイナリオープン
		const std::string filePath = topPath + "/" + tblDigitalWatchImage[0].filePath_;
		std::ifstream fin(filePath.c_str(), std::ios::in | std::ios::binary);
		if (fin) {
			//オープン成功

			//データサイズを取得
			fin.seekg(0, std::ios::end);
			dataSize = int32_t(fin.tellg());
			fin.seekg(0);

			data = new uint8_t[dataSize];
			fin.read(reinterpret_cast<char*>(data), dataSize);
		}

		ui::Image image = { 0 };
		image.id_ = tblDigitalWatchImage[0].id_;
		image.format_ = tblDigitalWatchImage[0].format_;
		image.isBlend_ = 0;
		image.body_.data_ = data;
		image.body_.dataSize_ = dataSize;

		//イメージをデコード
		ImageDecorder decorder;
		(void)decorder.decode(image);

		//デコード画像を取得
		std::int32_t decodeSize = 0;
		std::int32_t width = 0;
		std::int32_t height = 0;
		std::uint8_t* decode = decorder.getDecodeData(&decodeSize, &width, &height);

		//描画開始
		this->drawIF_->makeCurrent(true);

		//描画準備
		this->drawIF_->setup();

		//背景色
		ui::ColorUB backColor = { 125, 125, 125, 255 };
		this->drawIF_->clear(backColor);

		//画像描画
		ui::CoordI point = { 0, 0 };
		//this->drawIF_->drawImage(point, decode, width, height);

		/*
		//画像描画(テキストテスト)
		test_freetype();
		std::uint8_t* test = new uint8_t[HEIGHT * WIDTH * 4];
		std::int32_t ofs = 0;
		for (int32_t h = 0; h < HEIGHT; h++) {
			for (int32_t w = 0; w < WIDTH; w++) {
				test[ofs + 0] = txtimage[h][w];
				test[ofs + 1] = txtimage[h][w];
				test[ofs + 2] = txtimage[h][w];
				test[ofs + 3] = 255;
				ofs += 4;
			}
		}
		this->drawIF_->drawImage(point, test, WIDTH, HEIGHT);
		delete[] test;
		*/

		//文字描画
		this->drawIF_->drawString(point, L"東京ドーム　アトラクション");

		//描画終了
		this->drawIF_->swapBuffers();
		this->drawIF_->makeCurrent(false);

		delete[] data;
	}
}