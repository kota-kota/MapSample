#include "Str.hpp"

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