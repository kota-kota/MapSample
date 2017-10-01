#include "Image.hpp"


//----------------------------------------------------------
//
// 画像クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::Image::Image() :
	body_(nullptr), bodySize_(0), blend_(nullptr), blendSize_(0), type_(ImageType::UNKNOWN), width_(0), height_(0)
{
}

//デストラクタ
fw::Image::~Image()
{
	this->Free();
}

//本体データ作成
bool fw::Image::CreateBody(const std::byte_t* const body, const size_t bodySize, const ImageType type, const std::uint32_t width, const std::uint32_t height)
{
	bool isCreate = false;

	if (this->body_ == nullptr) {
		//未作成の場合は作成
		this->body_ = new std::byte_t[bodySize];
		(void)memcpy_s(this->body_, bodySize, body, bodySize);
		this->bodySize_ = bodySize;
		this->type_ = type;
		this->width_ = width;
		this->height_ = height;

		isCreate = true;
	}
	else {
		//すでに作成済みの場合は何もせずエラー
	}

	return isCreate;
}

//ブレンドデータ作成
bool fw::Image::CreateBlend(const std::byte_t* const blend, const size_t blendSize, const ImageType type)
{
	bool isCreate = false;

	if (this->body_ == nullptr) {
		//本体データ未作成
		goto END;
	}
	if (this->type_ != type) {
		//本体データと画像タイプが一致しない
		goto END;
	}

	if (this->blend_ == nullptr) {
		//未作成の場合は作成
		this->blend_ = new std::byte_t[blendSize];
		(void)memcpy_s(this->blend_, blendSize, blend, blendSize);
		this->blendSize_ = blendSize;

		isCreate = true;
	}
	else {
		//すでに作成済みの場合は何もせずエラー
	}

END:
	return isCreate;
}

//解放
void fw::Image::Free()
{
	if (this->body_ != nullptr) {
		delete[] this->body_;
		this->body_ = nullptr;
	}
	if (this->blend_ != nullptr) {
		delete[] this->blend_;
		this->blend_ = nullptr;
	}
	this->bodySize_ = 0;
	this->blendSize_ = 0;
	this->type_ = UNKNOWN;
	this->width_ = 0;
	this->height_ = 0;
}



//----------------------------------------------------------
//
// 画像ライブラリクラス
//
//----------------------------------------------------------

//ファイルから画像オブジェクト作成(body: [true]本体画像 [false]ブレンド画像)
bool fw::ImageLib::CreateImage(Image* const outImage, const std::string& file, const ImageType type, const bool body)
{
	bool isCreate = false;

	//画像ファイルオープン
	FILE* fp;
	(void)fopen_s(&fp, file.c_str(), "rb");
	if (fp == nullptr) {
		//オープン失敗
		goto END;
	}

	//画像データサイズ取得
	fpos_t pos;
	(void)fseek(fp, 0, SEEK_END);
	(void)fgetpos(fp, &pos);
	(void)fseek(fp, 0, SEEK_SET);
	size_t dataSize = size_t(pos);

	//画像データのメモリ確保
	std::byte_t* data = new std::byte_t[dataSize];

	//画像データ読み込み
	size_t readSize = fread_s(data, dataSize, 1, dataSize, fp);
	if (readSize == dataSize) {
		//読み込み成功

			//バイナリデータから画像オブジェクト作成
		isCreate = CreateImage(outImage, data, dataSize, type, body);

	}

	//本体データのメモリ解放
	delete[] data;

	//ファイルクローズ
	fclose(fp);

END:
	return isCreate;
}

//バイナリデータから画像オブジェクト作成(body: [true]本体画像 [false]ブレンド画像)
bool fw::ImageLib::CreateImage(Image* const outImage, const std::byte_t* const data, const size_t dataSize, const ImageType type, const bool body, const std::uint32_t width, const std::uint32_t height)
{
	bool isCreate = false;

	if (body) {
		//本体データ

		//画像幅高さを取得
		std::uint32_t w = width;
		std::uint32_t h = height;
		switch (type) {
		case RGB888:	//RGB888画像
		case RGBA8888:	//RGBA8888画像
			if ((w == 0) || (h == 0)) {
				//幅高さ指定がない場合はエラー
				goto END;
			}
			break;
		case BMP:		//BMP画像
			Bitmap::GetWH(data, &w, &h);
			break;
		case PNG:		//PNG画像
			break;
		default:		//未サポート
			goto END;
		};

		//本体データを作成
		isCreate = outImage->CreateBody(data, dataSize, type, w, h);
	}
	else {
		//ブレンドデータ

		if (outImage->Body() != nullptr) {
			//ブレンドデータを作成
			isCreate = outImage->CreateBlend(data, dataSize, type);
		}
		else {
			//本体データがなければエラー
		}
	}

END:
	return isCreate;
}

//Bitmap画像からRGBA8888画像へデコード
void fw::ImageLib::DecodeRgba8888FromBitmap(Image* const outImage, const Image& inImage)
{
	//RGBA8888画像の1ピクセルあたりのバイト数
	const uint32_t BYTE_PER_PIXEL = 4;

	//本体画像デコード
	std::uint32_t bodyDataSize = 0;
	std::byte_t* bodyData = nullptr;
	if (inImage.Body() != nullptr) {
		if (inImage.Type() == BMP) {
			//デコード後のRGBA8888画像のメモリ確保
			bodyDataSize = inImage.Width() * inImage.Height() * BYTE_PER_PIXEL;
			bodyData = new std::byte_t[bodyDataSize];

			//デコード
			Bitmap::DecodeRgba8888(&bodyData, inImage.Body());
		}
	}

	//ブレンド画像デコード
	std::uint32_t blendDataSize = 0;
	std::byte_t* blendData = nullptr;
	if (inImage.Blend() != nullptr) {
		//デコード後のRGBA8888画像のメモリ確保
		blendDataSize = inImage.Width() * inImage.Height() * BYTE_PER_PIXEL;
		blendData = new std::byte_t[blendDataSize];

		//デコード
		Bitmap::DecodeRgba8888(&blendData, inImage.Blend());
	}

	//本体画像とブレンド画像が両方あればブレンド処理
	if ((bodyData != nullptr) && (blendData != nullptr)) {
	}

	//本体画像から出力するRGBA8888画像オブジェクトを作成
	outImage->CreateBody(bodyData, bodyDataSize, RGBA8888, inImage.Width(), inImage.Height());

	//確保したメモリを解放
	if (bodyData != nullptr) {
		delete[] bodyData;
	}
	if (blendData != nullptr) {
		delete[] blendData;
	}
}
