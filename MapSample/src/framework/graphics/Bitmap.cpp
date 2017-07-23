#include "Bitmap.hpp"

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::vector;
using cmn::Color;
using graphics::Bitmap;
using graphics::BitmapType;

//ビットマップファイルヘッダ(Windows,OS/2共通)
static const int16_t BFH_HEADERSIZE = 14;
static const int16_t BFH_FILETYPE_OFS = 0;		//ファイルタイプ
static const int16_t BFH_FILESIZE_OFS = 2;		//ファイルサイズ[byte]
static const int16_t BFH_RESERVED1_OFS = 6;		//予約領域1
static const int16_t BFH_RESERVED2_OFS = 8;		//予約領域2
static const int16_t BFH_IMAGEOFS_OFS = 10;		//ファイル先頭から画像データまでのオフセット[byte]

//ビットマップ情報ヘッダ(Windows)
static const int16_t BIH_HEADERSIZE = 40;
static const int16_t BIH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
static const int16_t BIH_WIDTH_OFS = 18;		//画像の幅[pixel]
static const int16_t BIH_HEIGHT_OFS = 22;		//画像の高さ[pixel]
static const int16_t BIH_PLANENUM_OFS = 26;		//プレーン数
static const int16_t BIH_BITCOUNT_OFS = 28;		//色ビット数[bit]
static const int16_t BIH_COMPRESSION_OFS = 30;	//圧縮形式
static const int16_t BIH_IMGDATASIZE_OFS = 34;	//画像データサイズ[byte]
static const int16_t BIH_XDPM_OFS = 38;			//水平解像度[dot/m]
static const int16_t BIH_YDPM_OFS = 42;			//垂直解像度[dot/m]
static const int16_t BIH_PALLETENUM_OFS = 46;	//パレット数[使用色数]
static const int16_t BIH_IMPCOLORNUM_OFS = 50;	//重要色数
static const int16_t BIH_PALLETE_OFS = 54;		//パレット

//ビットマップコアヘッダ(OS/2)
static const int16_t BCH_HEADERSIZE = 12;
static const int16_t BCH_HEADERSIZE_OFS = 14;	//情報ヘッダサイズ[byte]
static const int16_t BCH_WIDTH_OFS = 18;		//画像の幅[pixel]
static const int16_t BCH_HEIGHT_OFS = 20;		//画像の高さ[pixel]
static const int16_t BCH_PLANENUM_OFS = 22;		//プレーン数
static const int16_t BCH_BITCOUNT_OFS = 24;		//色ビット数[bit]

//コンストラクタ
Bitmap::Bitmap()
	: width_(0), height_(0), bytePerPixel_(0), raw_(), bitmap_()
{
}

//デストラクタ
Bitmap::~Bitmap()
{
	this->width_ = 0;
	this->height_ = 0;
	this->bytePerPixel_ = 0;
	this->raw_.clear();
	this->bitmap_.clear();
}

//RAWデータから作成
bool Bitmap::create(const vector<uint8_t>& raw, const int16_t width, const int16_t height, const int8_t bytePerPixel)
{
	if (!this->raw_.empty()) {
		//RAWデータ作成済み
		return false;
	}
	if (!this->bitmap_.empty()) {
		//BITMAPデータ作成済み
		return false;
	}

	this->width_ = width;
	this->height_ = height;
	this->bytePerPixel_ = bytePerPixel;
	this->raw_ = raw;

	return true;
}

//BITMAPデータから作成
bool Bitmap::create(const vector<uint8_t>& bitmap)
{
	if (!this->raw_.empty()) {
		//RAWデータ作成済み
		return false;
	}
	if (!this->bitmap_.empty()) {
		//BITMAPデータ作成済み
		return false;
	}

	//BITMAPデータかチェック
	uint8_t fileType[2];
	this->read1ByteLe(bitmap, BFH_FILETYPE_OFS, fileType[0]);
	this->read1ByteLe(bitmap, BFH_FILETYPE_OFS + 1, fileType[1]);
	if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
		//BITMAPデータでない
		return false;
	}

	this->bitmap_ = bitmap;

	return true;
}

//エンコード(RAWデータからBITMAPデータへ変換)
bool Bitmap::encode(const BitmapType bitmapType)
{
	if (this->raw_.empty()) {
		//RAWデータ未作成
		return false;
	}
	if (!this->bitmap_.empty()) {
		//BITMAPデータ作成済み
		return false;
	}

	//色ビット数、ファイルヘッダサイズ、情報ヘッダサイズ
	uint16_t bitCount = 0;
	int16_t fileHeaderSize = 0;
	int16_t infoHeaderSize = 0;

	switch (bitmapType) {
	case WIN_32BIT:
		bitCount = 32;
		fileHeaderSize = BFH_HEADERSIZE;
		infoHeaderSize = BIH_HEADERSIZE;
		break;

	default:
		return false;
	};

	//パレットサイズを計算
	uint32_t palleteSize = 0;
	if (bitCount <= 8) {
		palleteSize = (1 << bitCount) * 4;
	}

	//画像の幅のパディング
	//ビット数も考慮しないといけない？
	uint32_t padding = 0;
	if (this->width_ % 4 != 0) {
		padding = ((this->width_ / 4 + 1) * 4) - this->width_;
	}

	//画像サイズを計算
	uint32_t imageSize = (this->width_ + padding) * this->height_;

	//ビットマップサイズを計算
	uint32_t bitmapSize = fileHeaderSize + infoHeaderSize + palleteSize + imageSize;

	//ビットマップサイズ分の領域を確保
	this->bitmap_.resize(bitmapSize);

	//ファイルタイプを設定
	this->write1ByteLe(this->bitmap_, BFH_FILETYPE_OFS, 'B');
	this->write1ByteLe(this->bitmap_, BFH_FILETYPE_OFS + 1, 'M');

	//ファイルサイズを設定
	this->write4ByteLe(this->bitmap_, BFH_FILESIZE_OFS, bitmapSize);

	//予約領域1を設定
	this->write2ByteLe(this->bitmap_, BFH_RESERVED1_OFS, 0);

	//予約領域2を設定
	this->write2ByteLe(this->bitmap_, BFH_RESERVED2_OFS, 0);

	//ファイル先頭から画像データまでのオフセットを設定
	int32_t imageOffset = fileHeaderSize + infoHeaderSize + palleteSize;
	this->write4ByteLe(this->bitmap_, BFH_IMAGEOFS_OFS, imageOffset);

	//情報ヘッダサイズを設定
	this->write4ByteLe(this->bitmap_, BIH_HEADERSIZE_OFS, infoHeaderSize);

	//画像の幅を設定
	this->write4ByteLe(this->bitmap_, BIH_WIDTH_OFS, this->width_ + padding);

	//画像の高さを設定
	this->write4ByteLe(this->bitmap_, BIH_HEIGHT_OFS, this->height_);

	//プレーン数を設定
	this->write2ByteLe(this->bitmap_, BIH_PLANENUM_OFS, 1);

	//色ビット数を設定
	this->write2ByteLe(this->bitmap_, BIH_PLANENUM_OFS, bitCount);

	//圧縮形式を設定
	this->write4ByteLe(this->bitmap_, BIH_COMPRESSION_OFS, 0);

	//画像データサイズを設定
	this->write4ByteLe(this->bitmap_, BIH_IMGDATASIZE_OFS, imageSize);

	//水平解像度を設定
	this->write4ByteLe(this->bitmap_, BIH_XDPM_OFS, 0);

	//垂直解像度を設定
	this->write4ByteLe(this->bitmap_, BIH_YDPM_OFS, 0);

	//パレット数を設定
	this->write4ByteLe(this->bitmap_, BIH_PALLETENUM_OFS, 0);

	//重要色数を設定
	this->write4ByteLe(this->bitmap_, BIH_IMPCOLORNUM_OFS, 0);

	//パレットデータを設定
	if (palleteSize > 0) {
		//どうする？
	}

	//画像データを設定
	/*
	int32_t writeIndex = fileHeaderSize + infoHeaderSize + palleteSize;
	for (int32_t h = this->height_ - 1; h >= 0; h--) {
		int32_t readIndex = h * this->width_ * this->mBitCount;
		for (int32_t w = 0; w < this->width_; w++) {
			for (int32_t bc = 0; bc < this->mBitCount; bc++) {
				this->write1ByteLe(&this->bitmap_, writeIndex, this->raw_[readIndex]);
				writeIndex++;
				readIndex++;
			}
		}
		for (uint32_t w = 0; w < padding; w++) {
			this->write1ByteLe(&this->bitmap_, writeIndex, 0);
			writeIndex++;
		}
	}
	*/

	return true;
}

//デコード(BITMAPデータからRAWデータへ変換)
bool Bitmap::decode()
{
	if (this->bitmap_.empty()) {
		//BITMAPデータ未作成
		return false;
	}
	if (!this->raw_.empty()) {
		//RAWデータ作成済み
		return false;
	}

	//ファイルタイプを取得
	uint8_t fileType[2];
	this->read1ByteLe(this->bitmap_, BFH_FILETYPE_OFS, fileType[0]);
	this->read1ByteLe(this->bitmap_, BFH_FILETYPE_OFS + 1, fileType[1]);
	if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
		//BITMAPデータでない
		return false;
	}

	//ファイル先頭から画像データまでのオフセットを取得
	uint32_t imageOffset;
	this->read4ByteLe(this->bitmap_, BFH_IMAGEOFS_OFS, imageOffset);

	//情報ヘッダサイズを取得
	uint32_t infoHeaderSize;
	this->read4ByteLe(this->bitmap_, BIH_HEADERSIZE_OFS, infoHeaderSize);

	if (infoHeaderSize == BIH_HEADERSIZE) {
		//Windowsフォーマット

		//画像の幅と高さを取得
		uint32_t width, height;
		this->read4ByteLe(this->bitmap_, BIH_WIDTH_OFS, width);
		this->read4ByteLe(this->bitmap_, BIH_HEIGHT_OFS, height);

		//色ビット数を取得
		uint16_t bitCount;
		this->read2ByteLe(this->bitmap_, BIH_BITCOUNT_OFS, bitCount);

		//パレット数を取得
		uint32_t palleteNum;
		this->read4ByteLe(this->bitmap_, BIH_PALLETENUM_OFS, palleteNum);
		if ((palleteNum == 0) && (bitCount <= 8)) {
			//パレット数が0かつビット数が8以下の場合は、ビット数からパレット数を計算
			palleteNum = (1 << bitCount);
		}

		//パレットデータを確保
		vector<Color> pallete;
		pallete.resize(palleteNum);

		//パレットデータを取得
		uint32_t readIndex = BIH_PALLETE_OFS;
		for (uint32_t i = 0; i < palleteNum; i++) {
			this->read1ByteLe(this->bitmap_, readIndex + 0, pallete[i].b);
			this->read1ByteLe(this->bitmap_, readIndex + 1, pallete[i].g);
			this->read1ByteLe(this->bitmap_, readIndex + 2, pallete[i].r);
			this->read1ByteLe(this->bitmap_, readIndex + 3, pallete[i].a);
			readIndex += 4;
		}

		//ビットマップデータを取得
		size_t imageSize = width * height * 4;
		this->raw_.resize(imageSize);
		for (uint32_t h = 0; h < height; h++) {
			uint32_t offset = (height - h - 1) * width * 4;
			for (uint32_t w = 0; w < width; w++) {
				uint8_t index;
				this->read1ByteLe(this->bitmap_, imageOffset, index);
				imageOffset++;

				this->raw_[offset + 0] = pallete[index].r;
				this->raw_[offset + 1] = pallete[index].g;
				this->raw_[offset + 2] = pallete[index].b;
				this->raw_[offset + 3] = 255;
				offset += 4;
			}
		}

		this->width_ = static_cast<int16_t>(width);
		this->height_ = static_cast<int16_t>(height);
		this->bytePerPixel_ = 4;
	}
	else if (infoHeaderSize == BCH_HEADERSIZE) {
		//OS/2フォーマット

		//画像の幅と高さを取得
		uint16_t width, height;
		this->read2ByteLe(this->bitmap_, BCH_WIDTH_OFS, width);
		this->read2ByteLe(this->bitmap_, BCH_HEIGHT_OFS, height);

		//色ビット数を取得
		uint16_t bitCount;
		this->read2ByteLe(this->bitmap_, BCH_BITCOUNT_OFS, bitCount);
	}
	else {
		//異常なBITMAPデータ
		return false;
	}

	return true;
}

//RAWデータ取得
bool Bitmap::getRaw(std::vector<std::uint8_t>& raw)
{
	if (this->raw_.empty()) {
		//RAWデータ未作成
		return false;
	}

	raw = this->raw_;

	return true;
}

//BITMAPデータ取得
bool Bitmap::getBitmap(std::vector<std::uint8_t>& bitmap)
{
	if (this->bitmap_.empty()) {
		//BITMAPデータ未作成
		return false;
	}

	bitmap = this->bitmap_;

	return true;
}

//RAWデータ情報取得
void Bitmap::getRawInfo(int16_t& width, int16_t& height, int8_t& bytePerPixel)
{
	width = this->width_;
	height = this->height_;
	bytePerPixel = this->bytePerPixel_;
}


//1バイトを書き込み(LE)
void Bitmap::write1ByteLe(vector<uint8_t>& data, const int32_t writeIndex, const uint8_t writeData)
{
	data[writeIndex + 0] = writeData;
}

//2バイトを書き込み(LE)
void Bitmap::write2ByteLe(vector<uint8_t>& data, const int32_t writeIndex, const uint16_t writeData)
{
	data[writeIndex + 0] = (writeData >> 0) & 0xFF;
	data[writeIndex + 1] = (writeData >> 8) & 0xFF;
}

//4バイトを書き込み(LE)
void Bitmap::write4ByteLe(vector<uint8_t>& data, const int32_t writeIndex, const uint32_t writeData)
{
	data[writeIndex + 0] = (writeData >> 0) & 0xFF;
	data[writeIndex + 1] = (writeData >> 8) & 0xFF;
	data[writeIndex + 2] = (writeData >> 16) & 0xFF;
	data[writeIndex + 3] = (writeData >> 24) & 0xFF;
}

//1バイトを読み込み(LE)
void Bitmap::read1ByteLe(const vector<uint8_t>& data, const int32_t readIndex, uint8_t& readData)
{
	readData = data[readIndex + 0];
}

//2バイトを読み込み(LE)
void Bitmap::read2ByteLe(const vector<uint8_t>& data, const int32_t readIndex, uint16_t& readData)
{
	readData = 0;

	uint16_t value = 0;

	value = static_cast<uint16_t>(data[readIndex + 0]);
	readData |= value << 0;
	value = static_cast<uint16_t>(data[readIndex + 1]);
	readData |= value << 8;
}

//4バイトを読み込み(LE)
void Bitmap::read4ByteLe(const vector<uint8_t>& data, const int32_t readIndex, uint32_t& readData)
{
	readData = 0;

	uint32_t value = 0;

	value = static_cast<uint32_t>(data[readIndex + 0]);
	readData |= value << 0;
	value = static_cast<uint32_t>(data[readIndex + 1]);
	readData |= value << 8;
	value = static_cast<uint32_t>(data[readIndex + 2]);
	readData |= value << 16;
	value = static_cast<uint32_t>(data[readIndex + 3]);
	readData |= value << 24;
}
