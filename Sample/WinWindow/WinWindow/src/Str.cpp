#include "Str.hpp"
#include <memory>


namespace {

	//文字数を取得
	std::int32_t getCharNum(const std::char8_t* const str, const std::int32_t size, const draw::EN_CharCode charCode)
	{
		std::int32_t charNum = 0;

		if (charCode == draw::EN_CharCode::SJIS) {
			//SJIS
			std::int32_t rlen = 0;
			while (rlen < size) {
				const std::uint8_t c = std::uint8_t(str[rlen]);

				//文字数加算
				charNum++;

				if (((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC))) {
					//SJIS全角文字の1バイト目なら2バイト文字
					rlen += 2;
				}
				else {
					//SJIS半角文字なら1バイト文字
					rlen += 1;
				}
			}
		}
		else {
		}

		return charNum;
	}
}

namespace draw {

	/**
	* 文字列クラス
	*/

	//コンストラクタ
	Str::Str(const std::char8_t* const str, const std::int32_t size, const EN_CharCode charCode) :
		str_(), size_(0), num_(0), charCode_(UNKNOWN)
	{
		memset(&this->str_[0], '\0', STR_MAXSIZE);
		if (str != nullptr) {
			//終端文字分を考慮してサイズチェック
			if (size < STR_MAXSIZE) {
				//文字列をコピー
				memcpy_s(&this->str_[0], STR_MAXSIZE, str, size);

				//文字サイズ
				this->size_ = size;
				//文字数
				this->num_ = getCharNum(str, size, charCode);
				//文字コード
				this->charCode_ = charCode;
			}
		}
	}

	//デストラクタ
	Str::~Str()
	{
	}
}

#if 0
namespace {
	//*********************************************
	//  変換テーブルを使ってSJIS→UTF16に文字コードを変換します
	//*********************************************
	long	SJIS2UTF16Tbl(long code)
	{
		return((((long)sjis2utf16_bin[2 * code]) << 8) + (long)sjis2utf16_bin[2 * code + 1]);
	}
}

namespace draw {
	//**************************************************
	//  SJIS文字コードをUTF16文字コードに変更します
	//  sjis:SJISの文字列, utf:UTF-16の文字列, UTF-16の最大バイト数
	//**************************************************
	static int SJIS2UTF16(unsigned char *sjis, unsigned char *utf, int byte)
	{
		int		codebyte = 0;
		long	utf16code = 0;
		long	fl;

		while (true) {
			if (*sjis == 0) {
				utf[codebyte] = 0;
				break;
			}
			//半角か全角のチェック
			if (((*sjis >> 7) & (1 - (((*sjis & 0x40) >> 6) ^ ((*sjis & 0x20) >> 5)))) == 0) {
				//半角だった
				utf16code = SJIS2UTF16Tbl((long)*sjis);
				sjis++;
			}
			else {
				//SJIS文字の1バイト目だった
				fl = (long)*sjis * 256L;
				sjis++;
				utf16code = SJIS2UTF16Tbl(fl + (long)*sjis);
				sjis++;
			}
			if (codebyte + 2 >= byte || utf16code == 0) {
				utf[codebyte] = 0;
				break;
			}
			utf[codebyte] = (unsigned char)(utf16code >> 8);
			utf[codebyte + 1] = (unsigned char)utf16code & 0xFF;
			codebyte += 2;
		}
		return(codebyte);
	}
}
#endif