#ifndef INCLUDED_BITMAP
#define INCLUDED_BITMAP

#include "Common.hpp"

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
		cmn::Int16_t	width_;			//幅
		cmn::Int16_t	height_;		//高さ
		cmn::Int8_t		bytePerPixel_;	//1ピクセルあたりのバイト数
		cmn::Image		raw_;			//RAWデータ
		cmn::Image		bitmap_;		//BITMAPデータ

	public:
		//コンストラクタ
		Bitmap();
		//デストラクタ
		~Bitmap();
		//RAWデータから作成
		bool create(const cmn::Image& raw, const cmn::Int16_t width, const cmn::Int16_t height, const cmn::Int8_t bytePerPixel);
		//BITMAPデータから作成
		bool create(const cmn::Image& bitmap);
		//エンコード(RAWデータからBITMAPデータへ変換)
		bool encode(const graphics::BitmapType bitmapType);
		//デコード(BITMAPデータからRAWデータへ変換)
		bool decode();
		//RAWデータ取得
		bool getRaw(cmn::Image& raw, cmn::Int16_t& width, cmn::Int16_t& height, cmn::Int8_t& bytePerPixel);
		//BITMAPデータ取得
		bool getBitmap(cmn::Image& bitmap);

	private:
		//1バイトを書き込み(LE)
		void write1ByteLe(cmn::Binary& data, const cmn::Int32_t writeIndex, const cmn::UInt8_t writeData);
		//2バイトを書き込み(LE)
		void write2ByteLe(cmn::Binary& data, const cmn::Int32_t writeIndex, const cmn::UInt16_t writeData);
		//4バイトを書き込み(LE)
		void write4ByteLe(cmn::Binary& data, const cmn::Int32_t writeIndex, const cmn::UInt32_t writeData);
		//1バイトを読み込み(LE)
		void read1ByteLe(const cmn::Binary& data, const cmn::Int32_t readIndex, cmn::UInt8_t& readData);
		//2バイトを読み込み(LE)
		void read2ByteLe(const cmn::Binary& data, const cmn::Int32_t readIndex, cmn::UInt16_t& readData);
		//4バイトを読み込み(LE)
		void read4ByteLe(const cmn::Binary& data, const cmn::Int32_t readIndex, cmn::UInt32_t& readData);
	};
}

#endif //INCLUDED_BITMAP
