#pragma once
#include <cstdint>
#include <vector>

namespace image
{
	enum BitmapType {
		WIN_1BIT,
		WIN_8BIT,
		WIN_24BIT,
		WIN_32BIT,
		OS2_1BIT,
		OS2_8BIT,
		OS2_24BIT,
		OS2_32BIT,
	};

	class Bitmap {
		class Color {
		public:
			std::uint8_t	r;
			std::uint8_t	g;
			std::uint8_t	b;
			std::uint8_t	a;
		};

		//メンバ変数
		std::int32_t				mWidth;			//幅
		std::int32_t				mHeight;		//高さ
		std::int32_t				mBytePerPixel;	//1ピクセルあたりのバイト数
		std::vector<std::uint8_t>	mRaw;			//RAWデータ
		std::vector<std::uint8_t>	mBitmap;		//BITMAPデータ

	public:
		//コンストラクタ
		Bitmap();
		//デストラクタ
		~Bitmap();
		//RAWデータから作成
		bool create(const std::vector<std::uint8_t>& raw, const std::int32_t width, const std::int32_t height, const std::int32_t bytePerPixel);
		//BITMAPデータから作成
		bool create(const std::vector<std::uint8_t>& bitmap);
		//エンコード(RAWデータからBITMAPデータへ変換)
		bool encode(const image::BitmapType bitmapType);
		//デコード(BITMAPデータからRAWデータへ変換)
		bool decode();
		//RAWデータ取得
		bool getRaw(std::vector<std::uint8_t>* raw);
		//BITMAPデータ取得
		bool getBitmap(std::vector<std::uint8_t>* bitmap);
		//RAWデータ情報取得
		void getRawInfo(std::int32_t* width, std::int32_t* height, std::int32_t* bytePerPixel);

	private:
		//1バイトを書き込み(LE)
		void write1ByteLe(std::vector<std::uint8_t>* data, const std::int32_t writeIndex, const std::uint8_t writeData);
		//2バイトを書き込み(LE)
		void write2ByteLe(std::vector<std::uint8_t>* data, const std::int32_t writeIndex, const std::uint16_t writeData);
		//4バイトを書き込み(LE)
		void write4ByteLe(std::vector<std::uint8_t>* data, const std::int32_t writeIndex, const std::uint32_t writeData);
		//1バイトを読み込み(LE)
		void read1ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint8_t* readData);
		//2バイトを読み込み(LE)
		void read2ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint16_t* readData);
		//4バイトを読み込み(LE)
		void read4ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint32_t* readData);
	};
}
