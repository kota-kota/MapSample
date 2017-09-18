#include "FwStd.hpp"


//----------------------------------------------------------
//
// バイナリ操作メソッド群
//
//----------------------------------------------------------

//1バイトを書き込み(LE)
void fw::BinWriter::write1ByteLe(std::uint8_t* const data, const std::uint8_t writeData)
{
	*(data + 0) = writeData;
}

//2バイトを書き込み(LE)
void fw::BinWriter::write2ByteLe(std::uint8_t* const data, const std::uint16_t writeData)
{
	*(data + 0) = (writeData >> 0) & 0xFF;
	*(data + 1) = (writeData >> 8) & 0xFF;
}

//4バイトを書き込み(LE)
void fw::BinWriter::write4ByteLe(std::uint8_t* const data, const std::uint32_t writeData)
{
	*(data + 0) = (writeData >> 0) & 0xFF;
	*(data + 1) = (writeData >> 8) & 0xFF;
	*(data + 2) = (writeData >> 16) & 0xFF;
	*(data + 3) = (writeData >> 24) & 0xFF;
}

//1バイトを読み込み(LE)
void fw::BinReader::read1ByteLe(const std::uint8_t* const data, std::uint8_t* const readData)
{
	*readData = *(data + 0);
}

//2バイトを読み込み(LE)
void fw::BinReader::read2ByteLe(const std::uint8_t* const data, std::uint16_t* const readData)
{
	*readData = 0;
	std::uint16_t v1 = std::uint16_t(*(data + 0));
	*readData |= v1 << 0;
	std::uint16_t v2 = std::uint16_t(*(data + 1));
	*readData |= v2 << 8;
}

//4バイトを読み込み(LE)
void fw::BinReader::read4ByteLe(const std::uint8_t* const data, std::uint32_t* const readData)
{
	*readData = 0;
	std::uint32_t v1 = std::uint32_t(*(data + 0));
	*readData |= v1 << 0;
	std::uint32_t v2 = std::uint32_t(*(data + 1));
	*readData |= v2 << 8;
	std::uint32_t v3 = std::uint32_t(*(data + 2));
	*readData |= v3 << 16;
	std::uint32_t v4 = std::uint32_t(*(data + 3));
	*readData |= v4 << 24;
}
