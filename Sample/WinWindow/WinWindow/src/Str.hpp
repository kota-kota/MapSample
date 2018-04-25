#ifndef INCLUDED_STR_HPP
#define INCLUDED_STR_HPP

#include "Type.hpp"


namespace draw {

	//最大文字サイズ
	const int32_t STR_MAXSIZE = 256;

	//文字コード
	enum EN_CharCode {
		UNKNOWN,
		SJIS,
		UTF8,
		UTF16LE,
	};

	/**
	* 文字列クラス
	*/
	class Str {
		std::char8_t	str_[STR_MAXSIZE];	//文字列
		std::int32_t	size_;				//文字サイズ(終端文字は含まない)
		std::int32_t	num_;				//文字数(終端文字は含まない)
		EN_CharCode		charCode_;			//文字コード

	public:
		//コンストラクタ
		Str(const std::char8_t* const str, const std::int32_t size, const EN_CharCode charCode);
		//デストラクタ
		~Str();

	private:
	};
}

#endif //INCLUDED_STR_HPP
