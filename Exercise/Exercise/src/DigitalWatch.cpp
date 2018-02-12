#include "DigitalWatch.hpp"
#include "DrawWGL.hpp"
#include "Image.hpp"
#include <fstream>
#include <string>

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
		this->drawIF_->drawImage(point, decode, width, height);

		//描画終了
		this->drawIF_->swapBuffers();
		this->drawIF_->makeCurrent(false);

		delete[] data;
	}
}