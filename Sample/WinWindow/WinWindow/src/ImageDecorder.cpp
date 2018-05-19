#include "ImageDecorder.hpp"
#include "ImageBMP.hpp"
#include "ImagePNG.hpp"
#include <fstream>


namespace {
	//RGBA8888画像の1ピクセルあたりのバイト数
	const std::int32_t BYTE_PER_PIXEL_RGBA8888 = 4;
}


namespace fw {

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
	std::int32_t ImageDecorder::decode_RGBA8888(const std::char8_t* const bodyFilePath, const std::char8_t* const blendFilePath, const EN_ImageFormat format, const bool isFlip)
	{
		std::int32_t rc = -1;

		//画像データ
		std::uint8_t* bodyData = nullptr;
		std::uint8_t* blendData = nullptr;
		std::int32_t bodyDataSize = 0;
		std::int32_t blendDataSize = 0;

		//本体画像ファイルオープン
		std::ifstream ifs_body(bodyFilePath, std::ios::binary);
		if (!ifs_body) {
			//オープン失敗
			goto END;
		}

		//本体画像データサイズ取得
		ifs_body.seekg(0, std::ios::end);
		bodyDataSize = static_cast<std::int32_t>(ifs_body.tellg());
		ifs_body.seekg(0, std::ios::beg);
		if (bodyDataSize <= 0) {
			//サイズ異常
			goto END;
		}

		//本体画像データ領域を確保し、ファイル読み込み
		bodyData = new std::uint8_t[bodyDataSize];
		ifs_body.read(reinterpret_cast<std::char8_t*>(bodyData), bodyDataSize);

		//ブレンド画像ファイルの指定があれば、ブレンド画像を読み込む
		if (blendFilePath != nullptr) {
			//ブレンド画像ファイルオープン
			std::ifstream ifs_blend(blendFilePath, std::ios::binary);
			if (!ifs_blend) {
				//オープン失敗
				goto END;
			}

			//ブレンド画像データサイズ取得
			ifs_blend.seekg(0, std::ios::end);
			blendDataSize = static_cast<std::int32_t>(ifs_blend.tellg());
			ifs_blend.seekg(0, std::ios::beg);
			if (blendDataSize <= 0) {
				//サイズ異常
				goto END;
			}

			//ブレンド画像データ領域を確保し、ファイル読み込み
			blendData = new std::uint8_t[blendDataSize];
			ifs_blend.read(reinterpret_cast<std::char8_t*>(blendData), blendDataSize);
		}

		//RGBA8888画像へデコード
		rc = this->decode_RGBA8888(bodyData, bodyDataSize, blendData, blendDataSize, format, isFlip);

	END:
		//画像データ読み込み用領域を解放
		if (bodyData != nullptr) {
			delete[] bodyData;
		}
		if (blendData != nullptr) {
			delete[] blendData;
		}
		return rc;
	}
	std::int32_t ImageDecorder::decode_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize, std::uint8_t* const blendData, const std::int32_t blendDataSize, const EN_ImageFormat format, const bool isFlip)
	{
		std::int32_t rc = -1;
		std::int32_t ret = -1;

		//以前のデコードデータがあれば解放
		if (this->decData_ != nullptr) {
			delete[] this->decData_;
		}

		//画像フォーマット毎の処理
		if (format == BMP) {
			//BMP画像

			//本体画像をデコード
			ret = this->decodeBMP_RGBA8888(bodyData, bodyDataSize);
			if (ret < 0) {
				goto END;
			}

			//ブレンド画像の指定があれば、ブレンド処理を実施
			if (blendData != nullptr) {
				ret = this->blendBMP_RGBA8888(blendData, blendDataSize);
				if (ret < 0) {
					goto END;
				}
			}
		}
		else if (format == PNG) {
			//PNG画像

			//本体画像をデコード
			ret = this->decodePNG_RGBA8888(bodyData, bodyDataSize);
			if (ret < 0) {
				goto END;
			}

			//ブレンド画像の指定があれば、ブレンド処理を実施
			if (blendData != nullptr) {
				ret = this->blendPNG_RGBA8888(blendData, blendDataSize);
				if (ret < 0) {
					goto END;
				}
			}
		}
		else {
		}

		if (isFlip) {
			//デコード結果を上下反転する
		}

		//正常終了
		rc = 0;

	END:
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

	//本体BMP画像をRGBA8888画像へデコード
	std::int32_t ImageDecorder::decodeBMP_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize)
	{
		std::int32_t rc = -1;
		std::int32_t ret = -1;

		//本体BMP画像オブジェクト生成
		ImageBMP bmp_body;
		ret = bmp_body.create(bodyData, bodyDataSize);
		if (ret < 0) {
			//生成失敗
			goto END;
		}

		//本体BMP画像幅高さを取得
		bmp_body.getWH(&this->width_, &this->height_);

		//デコードデータ格納領域を確保
		this->decDataSize_ = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
		this->decData_ = new std::uint8_t[this->decDataSize_];

		//本体BMP画像をRGBA8888画像へデコード
		ret = bmp_body.decode_RGBA8888(&this->decData_);
		if (ret < 0) {
			//デコード失敗
			goto END;
		}

		//正常終了
		rc = 0;

	END:
		return rc;
	}

	//ブレンドBMP画像をRGBA8888画像へデコードし、本体デコード画像へブレンド
	std::int32_t ImageDecorder::blendBMP_RGBA8888(std::uint8_t* const blendData, const std::int32_t blendDataSize)
	{
		std::int32_t rc = -1;
		std::int32_t ret = -1;

		std::uint8_t* decData_blend = nullptr;

		//ブレンドBMP画像オブジェクト生成
		ImageBMP bmp_blend;
		ret = bmp_blend.create(blendData, blendDataSize);
		if (ret < 0) {
			//生成失敗
			goto END;
		}

		//ブレンドBMP画像幅高さを取得
		std::int32_t width_blend, height_blend;
		bmp_blend.getWH(&width_blend, &height_blend);
		if ((this->width_ != width_blend) || (this->height_ != height_blend)) {
			//幅高さが不一致
			goto END;
		}

		//デコードデータ格納領域を確保
		decData_blend = new std::uint8_t[this->decDataSize_];

		//ブレンドBMP画像をRGBA8888画像へデコード
		ret = bmp_blend.decode_RGBA8888(&decData_blend);
		if (ret < 0) {
			//デコード失敗
			goto END;
		}

		//RGBA8888画像のブレンド処理
		this->blend_RGBA8888(decData_blend);

		//正常終了
		rc = 0;

	END:
		if (decData_blend != nullptr) {
			delete[] decData_blend;
		}
		return rc;
	}

	//本体PNG画像をRGBA8888画像へデコード
	std::int32_t ImageDecorder::decodePNG_RGBA8888(std::uint8_t* const bodyData, const std::int32_t bodyDataSize)
	{
		std::int32_t rc = -1;
		std::int32_t ret = -1;

		//本体PNG画像オブジェクト生成
		ImagePNG png_body;
		ret = png_body.create(bodyData, bodyDataSize);
		if (ret < 0) {
			//生成失敗
			goto END;
		}

		//本体PNG画像幅高さを取得
		png_body.getWH(&this->width_, &this->height_);

		//デコードデータ格納領域を確保
		this->decDataSize_ = this->width_ * this->height_ * BYTE_PER_PIXEL_RGBA8888;
		this->decData_ = new std::uint8_t[this->decDataSize_];

		//本体PNG画像をRGBA8888画像へデコード
		ret = png_body.decode_RGBA8888(&this->decData_);
		if (ret < 0) {
			//デコード失敗
			goto END;
		}

		//正常終了
		rc = 0;

	END:
		return rc;
	}

	//ブレンドPNG画像をRGBA8888画像へデコードし、本体デコード画像へブレンド
	std::int32_t ImageDecorder::blendPNG_RGBA8888(std::uint8_t* const blendData, const std::int32_t blendDataSize)
	{
		std::int32_t rc = -1;
		std::int32_t ret = -1;

		std::uint8_t* decData_blend = nullptr;

		//ブレンドPNG画像オブジェクト生成
		ImagePNG png_blend;
		ret = png_blend.create(blendData, blendDataSize);
		if (ret < 0) {
			//生成失敗
			goto END;
		}

		//ブレンドPNG画像幅高さを取得
		std::int32_t width_blend, height_blend;
		png_blend.getWH(&width_blend, &height_blend);
		if ((this->width_ != width_blend) || (this->height_ != height_blend)) {
			//幅高さが不一致
			goto END;
		}

		//デコードデータ格納領域を確保
		decData_blend = new std::uint8_t[this->decDataSize_];

		//ブレンドPNG画像をRGBA8888画像へデコード
		ret = png_blend.decode_RGBA8888(&decData_blend);
		if (ret < 0) {
			//デコード失敗
			goto END;
		}

		//RGBA8888画像のブレンド処理
		this->blend_RGBA8888(decData_blend);

		//正常終了
		rc = 0;

	END:
		if (decData_blend != nullptr) {
			delete[] decData_blend;
		}
		return rc;
	}


	//RGBA8888画像のブレンド処理
	void ImageDecorder::blend_RGBA8888(std::uint8_t* const decData_blend)
	{
		std::int32_t offset = 0;
		for (std::int32_t h = 0; h < this->height_; h++) {
			for (std::int32_t w = 0; w < this->width_; w++) {
				this->decData_[offset + 3] = decData_blend[offset];
				offset += BYTE_PER_PIXEL_RGBA8888;
			}
		}
	}
}
