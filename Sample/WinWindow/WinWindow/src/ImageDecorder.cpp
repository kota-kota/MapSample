#include "ImageDecorder.hpp"
#include "ImageBMP.hpp"
#include "ImagePNG.hpp"
#include <fstream>


namespace draw {

	/**
	 * 画像デコードクラス
	 */

	//コンストラクタ
	ImageDecorder::ImageDecorder() :
		decData_(nullptr), decDataSize_(0), width_(0), height_(0)
	{
	}

	//デストラクタ
	ImageDecorder::~ImageDecorder()
	{
		if (this->decData_ != nullptr) {
			delete[] this->decData_;
		}
	}

	//RGBA8888画像へデコード
	std::int32_t ImageDecorder::decode_RGBA8888(const std::char8_t* const filePath, const EN_ImageFormat format, const bool isFlip)
	{
		std::int32_t rc = 0;

		//画像ファイルオープン
		std::ifstream ifs(filePath, std::ios::binary);
		if(ifs) {
			//画像データサイズ取得
			ifs.seekg(0, std::ios::end);
			const std::int32_t dataSize = static_cast<std::int32_t>(ifs.tellg());
			ifs.seekg(0, std::ios::beg);
			if(dataSize > 0) {
				//画像データ読み込み用領域を確保
				std::uint8_t* data = new std::uint8_t[dataSize];
				//画像データ読み込み
				ifs.read(reinterpret_cast<std::char8_t*>(data), dataSize);

				//RGBA8888画像へデコード
				rc = this->decode_RGBA8888(data, dataSize, format, isFlip);

				//画像データ読み込み用領域を解放
				delete[] data;
			}
			else {
				//サイズ異常
				rc = -1;
			}
		}
		else {
			//オープン失敗
			rc = -1;
		}

		return rc;
	}
	std::int32_t ImageDecorder::decode_RGBA8888(std::uint8_t* const data, const std::int32_t dataSize, const EN_ImageFormat format, const bool isFlip)
	{
		std::int32_t rc = 0;

		//RGBA8888画像の1ピクセルあたりのバイト数
		const std::int32_t BYTE_PER_PIXEL_RGBA8888 = 4;

		//以前のデコードデータがあれば解放
		if (this->decData_ != nullptr) {
			delete[] this->decData_;
		}

		if (format == EN_ImageFormat::BMP) {
			//BMP画像

			//画像BMPオブジェクト生成
			ImageBMP bmp;
			rc = bmp.create(data, dataSize);
			if(rc == 0) {
				//画像幅高さを取得
				bmp.getWH(&this->width_, &this->height_);

				//デコードデータ格納領域を確保
				this->decDataSize_ = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
				this->decData_ = new std::uint8_t[this->decDataSize_];

				//BMP画像をRGBA8888画像へデコード
				rc = bmp.decode_RGBA8888(&this->decData_);
			}
		}
		else if (format == EN_ImageFormat::PNG) {
			//PNG画像

			//画像PNGオブジェクト生成
			ImagePNG png;
			rc = png.create(data, dataSize);
			if (rc == 0) {
				//画像幅高さを取得
				png.getWH(&this->width_, &this->height_);

				//デコードデータ格納領域を確保
				this->decDataSize_ = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
				this->decData_ = new std::uint8_t[this->decDataSize_];

				//BMP画像をRGBA8888画像へデコード
				rc = png.decode_RGBA8888(&this->decData_);
			}
		}

		if (rc == 0) {
			//デコード成功

			if (isFlip) {
				//デコード結果を上下反転する
			}
			
		}

		return rc;
	}

	//デコードデータ取得
	std::uint8_t* ImageDecorder::getDecodeData(std::int32_t* const decDataSize, std::int32_t* const width, std::int32_t* const height)
	{
		if (decDataSize != nullptr) { *decDataSize = this->decDataSize_; }
		if (width != nullptr) { *width = this->width_; }
		if (height != nullptr) { *height = this->height_; }
		return this->decData_;
	}

}
