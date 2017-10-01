#include "Std.hpp"


//----------------------------------------------------------
//
// バイナリ操作メソッド群
//
//----------------------------------------------------------

//1バイトを書き込み(LE)
void std::ByteWriter::write1ByteLe(byte_t* const data, const uint8_t writeData)
{
	*(data + 0) = writeData;
}

//2バイトを書き込み(LE)
void std::ByteWriter::write2ByteLe(byte_t* const data, const uint16_t writeData)
{
	*(data + 0) = (writeData >> 0) & 0xFF;
	*(data + 1) = (writeData >> 8) & 0xFF;
}

//4バイトを書き込み(LE)
void std::ByteWriter::write4ByteLe(byte_t* const data, const uint32_t writeData)
{
	*(data + 0) = (writeData >> 0) & 0xFF;
	*(data + 1) = (writeData >> 8) & 0xFF;
	*(data + 2) = (writeData >> 16) & 0xFF;
	*(data + 3) = (writeData >> 24) & 0xFF;
}

//1バイトを読み込み(LE)
void std::ByteReader::read1ByteLe(const byte_t* const data, uint8_t* const readData)
{
	*readData = *(data + 0);
}

//2バイトを読み込み(LE)
void std::ByteReader::read2ByteLe(const byte_t* const data, uint16_t* const readData)
{
	*readData = 0;
	uint16_t v1 = uint16_t(*(data + 0));
	*readData |= v1 << 0;
	uint16_t v2 = uint16_t(*(data + 1));
	*readData |= v2 << 8;
}

//4バイトを読み込み(LE)
void std::ByteReader::read4ByteLe(const byte_t* const data, uint32_t* const readData)
{
	*readData = 0;
	uint32_t v1 = uint32_t(*(data + 0));
	*readData |= v1 << 0;
	uint32_t v2 = uint32_t(*(data + 1));
	*readData |= v2 << 8;
	uint32_t v3 = uint32_t(*(data + 2));
	*readData |= v3 << 16;
	uint32_t v4 = uint32_t(*(data + 3));
	*readData |= v4 << 24;
}
