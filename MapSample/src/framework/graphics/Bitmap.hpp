#ifndef INCLUDED_BITMAP
#define INCLUDED_BITMAP

#include "Common.hpp"
#include <vector>

namespace graphics
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
		//メンバ変数
		std::int16_t				width_;			//幅
		std::int16_t				height_;		//高さ
		std::int8_t					bytePerPixel_;	//1ピクセルあたりのバイト数
		std::vector<std::uint8_t>	raw_;			//RAWデータ
		std::vector<std::uint8_t>	bitmap_;		//BITMAPデータ

	public:
		//コンストラクタ
		Bitmap();
		//デストラクタ
		~Bitmap();
		//RAWデータから作成
		bool create(const std::vector<std::uint8_t>& raw, const std::int16_t width, const std::int16_t height, const std::int8_t bytePerPixel);
		//BITMAPデータから作成
		bool create(const std::vector<std::uint8_t>& bitmap);
		//エンコード(RAWデータからBITMAPデータへ変換)
		bool encode(const graphics::BitmapType bitmapType);
		//デコード(BITMAPデータからRAWデータへ変換)
		bool decode();
		//RAWデータ取得
		bool getRaw(std::vector<std::uint8_t>& raw);
		//BITMAPデータ取得
		bool getBitmap(std::vector<std::uint8_t>& bitmap);
		//RAWデータ情報取得
		void getRawInfo(std::int16_t& width, std::int16_t& height, std::int8_t& bytePerPixel);

	private:
		//1バイトを書き込み(LE)
		void write1ByteLe(std::vector<std::uint8_t>& data, const std::int32_t writeIndex, const std::uint8_t writeData);
		//2バイトを書き込み(LE)
		void write2ByteLe(std::vector<std::uint8_t>& data, const std::int32_t writeIndex, const std::uint16_t writeData);
		//4バイトを書き込み(LE)
		void write4ByteLe(std::vector<std::uint8_t>& data, const std::int32_t writeIndex, const std::uint32_t writeData);
		//1バイトを読み込み(LE)
		void read1ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint8_t& readData);
		//2バイトを読み込み(LE)
		void read2ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint16_t& readData);
		//4バイトを読み込み(LE)
		void read4ByteLe(const std::vector<std::uint8_t>& data, const std::int32_t readIndex, std::uint32_t& readData);
	};
}

#endif //INCLUDED_BITMAP
