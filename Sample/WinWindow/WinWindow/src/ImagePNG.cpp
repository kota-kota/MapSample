#include "ImagePNG.hpp"
#include <cstdio>
#include <memory>


namespace {

	//RGBA8888画像の1ピクセルあたりのバイト数
	static const std::int32_t BYTE_PER_PIXEL_RGBA8888 = 4;

	//PNGシグネチャのバイト数
	static const std::int32_t PNG_BYTES_TO_CHECK = 4;

	//PNG読み込みコールバック関数
	static void callbackReadPng(png_structp pngStr, png_bytep data, png_size_t length)
	{
		png_bytepp buf = (png_bytepp)png_get_io_ptr(pngStr);
		memcpy_s(data, length, *buf, length);
		*buf += length;
	}

}	//end of namespace



namespace fw {

	/**
	 * 画像PNGクラス
	 */

	//コンストラクタ
	ImagePNG::ImagePNG() :
		png_(nullptr), pngSize_(0), width_(0), height_(0), rowByte_(0), bitDepth_(0), colorType_(0),
		pngStr_(nullptr), pngInfo_(nullptr)
	{
	}

	//デストラクタ
	ImagePNG::~ImagePNG()
	{
		if (this->pngInfo_ != nullptr) {
			png_destroy_info_struct(this->pngStr_, &this->pngInfo_);
		}
		if (this->pngStr_ != nullptr) {
			png_destroy_read_struct(&this->pngStr_, nullptr, nullptr);
		}
	}

	//作成
	std::int32_t ImagePNG::create(std::uint8_t* const png, const std::int32_t pngSize)
	{
		std::int32_t rc = 0;

		//メンバへ保持
		this->png_ = png;
		this->pngSize_ = pngSize;

		//PNGシグネチャのチェック
		png_byte sig[PNG_BYTES_TO_CHECK];
		(void)memcpy_s(sig, PNG_BYTES_TO_CHECK, this->png_, PNG_BYTES_TO_CHECK);
		if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK) == 0) {
			//PNG画像

			//PNG構造ポインタ作成
			this->pngStr_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

			//PNG情報ポインタ作成
			this->pngInfo_ = png_create_info_struct(this->pngStr_);

			//シグネチャ読み込み済み
			png_set_sig_bytes(this->pngStr_, PNG_BYTES_TO_CHECK);
			this->png_ += PNG_BYTES_TO_CHECK;

			//PNG読み込みコールバック関数を登録
			png_set_read_fn(this->pngStr_, &this->png_, callbackReadPng);

			//PNG読み込み
			png_read_info(this->pngStr_, this->pngInfo_);

			//IHDRチャンクの各種情報取得
			this->width_ = png_get_image_width(this->pngStr_, this->pngInfo_);
			this->height_ = png_get_image_height(this->pngStr_, this->pngInfo_);
			this->rowByte_ = int32_t(png_get_rowbytes(this->pngStr_, this->pngInfo_));
			this->bitDepth_ = png_get_bit_depth(this->pngStr_, this->pngInfo_);
			this->colorType_ = png_get_color_type(this->pngStr_, this->pngInfo_);
		}
		else {
			//PNG画像でない
			rc = -1;
		}

		return rc;
	}

	//幅高さ取得
	void ImagePNG::getWH(std::int32_t* const width, std::int32_t* const height)
	{
		if (width != nullptr) { *width = this->width_; }
		if (height != nullptr) { *height = this->height_; }
	}

	//RGBA8888画像へデコード
	std::int32_t ImagePNG::decode_RGBA8888(std::uint8_t** const decData)
	{
		std::int32_t rc = 0;

		if (this->pngStr_ != nullptr) {
			//デコード後の画像データを格納するメモリを確保(本関数の最後に解放)
			png_size_t pngSize = this->height_ * sizeof(png_bytep) + this->height_ * this->rowByte_;
			png_byte* tmp = new png_byte[pngSize];
			png_bytepp png = (png_bytepp)tmp;

			//png_read_imageには各行へのポインタを渡すため、2次元配列化
			png_bytep wp = (png_bytep)&png[this->height_];
			for (std::int32_t h = 0; h < this->height_; h++) {
				png[h] = wp;
				wp += this->rowByte_;
			}

			//PNGイメージ読み込み
			png_read_image(this->pngStr_, png);

			switch (this->colorType_) {
			case PNG_COLOR_TYPE_GRAY:		//0:グレー
				rc = this->decodeGrayScalePng_RGBA8888(decData, png);
				break;
			case PNG_COLOR_TYPE_RGB:		//2:トゥルーカラー
				rc = this->decodeTrueColorPng_RGBA8888(decData, png);
				break;
			case PNG_COLOR_TYPE_PALETTE:	//3:パレット
				rc = this->decodePalletePng_RGBA8888(decData, png);
				break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:	//4:グレー+アルファ
				break;
			case PNG_COLOR_TYPE_RGB_ALPHA:	//6:トゥルーカラー+アルファ
				break;
			default:
				break;
			};

			//デコード後の画像データを格納するメモリを解放
			delete[] png;
		}
		else {
			//PNG構造未作成
			rc = -1;
		}

		return rc;
	}

	//グレーPNG画像からRGBA8888画像へデコード
	std::int32_t ImagePNG::decodeGrayScalePng_RGBA8888(std::uint8_t** const decData, const png_bytepp png)
	{
		std::int32_t rc = 0;

		if (this->bitDepth_ <= 8) {
			//ビット深度が1bit,2bit,4bit,8bitの場合

			//ビット深度で表現できる最大値
			png_byte bitMaxValue = (0x01 << this->bitDepth_) - 1;

			//グレーサンプル値(輝度に応じたグレーカラー取得に必要)
			png_byte graySample = 255 / bitMaxValue;

			//出力データへデコード後の画像データを設定
			for (std::int32_t h = 0; h < this->height_; h++) {
				//一行ずつ処理
				png_bytep wp = png[h];

				//書き込み位置と読み込み位置を初期化
				std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
				std::int32_t readOffset = 0;

				//ビット深度に応じたビットオフセット
				png_int_16 bitOfs = 8 - this->bitDepth_;

				for (std::int32_t w = 0; w < this->width_; w++) {
					//輝度を取得
					png_byte brightness = (wp[readOffset] >> bitOfs) & bitMaxValue;

					//輝度に応じたグレーカラーを取得
					png_byte grayColor = graySample * brightness;

					//出力データへRGBA値を設定
					*((*decData) + writeOffset + 0) = grayColor;
					*((*decData) + writeOffset + 1) = grayColor;
					*((*decData) + writeOffset + 2) = grayColor;
					*((*decData) + writeOffset + 3) = 255;

					//ビットオフセットを更新
					bitOfs -= this->bitDepth_;
					if (bitOfs < 0) {
						//次の読み込み位置に更新
						bitOfs = 8 - this->bitDepth_;
						readOffset++;
					}
					//書き込み位置を更新
					writeOffset += BYTE_PER_PIXEL_RGBA8888;
				}
			}
		}
		else {
			//ビット深度が1bit,2bit,4bit,8bit以外の場合
			//ビット深度が16bitの可能性があるが、現状48bitカラーは表現できないので実装しない
			//16bit以外でここに来た場合は異常
			rc = -1;
		}

		return rc;
	}

	//トゥルーカラーPNG画像からRGBA8888画像へデコード
	std::int32_t ImagePNG::decodeTrueColorPng_RGBA8888(std::uint8_t** const decData, const png_bytepp png)
	{
		std::int32_t rc = 0;

		if (this->bitDepth_ == 8) {
			//ビット深度が8bitの場合

			//出力データへデコード後の画像データを設定
			for (std::int32_t h = 0; h < this->height_; h++) {
				//一行ずつ処理
				png_bytep wp = png[h];

				//書き込み位置と読み込み位置を初期化
				std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
				std::int32_t readOffset = 0;

				for (std::int32_t w = 0; w < this->width_; w++) {
					//出力データへRGBA値を設定
					*((*decData) + writeOffset + 0) = wp[readOffset + 0];
					*((*decData) + writeOffset + 1) = wp[readOffset + 1];
					*((*decData) + writeOffset + 2) = wp[readOffset + 2];
					*((*decData) + writeOffset + 3) = 255;

					//書き込み位置を更新
					writeOffset += BYTE_PER_PIXEL_RGBA8888;
					readOffset += 3;
				}
			}
		}
		else {
			//ビット深度が8bit以外は何もしない
			//ビット深度が16bitの可能性があるが、現状48bitカラーは表現できないので実装しない
			//16bit以外でここに来た場合は異常
			rc = -1;
		}

		return rc;
	}

	//パレットPNG画像からRGBA8888画像へデコード
	std::int32_t ImagePNG::decodePalletePng_RGBA8888(std::uint8_t** const decData, const png_bytepp png)
	{
		std::int32_t rc = 0;

		//PLTEチャンク読み込み
		png_colorp pallete = nullptr;
		std::int32_t palleteNum = 0;
		png_get_PLTE(this->pngStr_, this->pngInfo_, &pallete, &palleteNum);

		if ((pallete != nullptr) && (palleteNum > 0)) {
			//パレットデータ取得成功

			//出力データへデコード後の画像データを設定
			for (std::int32_t h = 0; h < this->height_; h++) {
				//一行ずつ処理
				png_bytep wp = png[h];

				//書き込み位置と読み込み位置を初期化
				std::int32_t writeOffset = h * this->width_ * BYTE_PER_PIXEL_RGBA8888;
				std::int32_t readOffset = 0;

				//ビット深度に応じたビットオフセットとビットマスク
				png_int_16 bitOfs = 8 - this->bitDepth_;
				png_byte bitMask = (0x01 << this->bitDepth_) - 1;

				for (std::int32_t w = 0; w < this->width_; w++) {
					//画像データはパレットインデックス
					png_byte palleteIndex = (wp[readOffset] >> bitOfs) & bitMask;

					//出力データへRGBA値を設定
					*((*decData) + writeOffset + 0) = pallete[palleteIndex].red;
					*((*decData) + writeOffset + 1) = pallete[palleteIndex].green;
					*((*decData) + writeOffset + 2) = pallete[palleteIndex].blue;
					*((*decData) + writeOffset + 3) = 255;

					//ビットオフセットを更新
					bitOfs -= this->bitDepth_;
					if (bitOfs < 0) {
						//次の読み込み位置に更新
						bitOfs = 8 - this->bitDepth_;
						readOffset++;
					}
					//書き込み位置を更新
					writeOffset += BYTE_PER_PIXEL_RGBA8888;
				}
			}
		}
		else {
			//パレットデータ取得失敗
			rc = -1;
		}

		return rc;
	}

}	//end of namespace
