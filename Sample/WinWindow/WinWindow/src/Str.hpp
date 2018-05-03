#ifndef INCLUDED_STR_HPP
#define INCLUDED_STR_HPP

#include "Type.hpp"
#include <string>


namespace fw {

	//文字コード
	enum EN_CharCode {
		UNKNOWN,
		SJIS,
		UTF8,
		UTF16BE,
		UTF16LE,
	};

	/**
	* 文字列クラス
	*/
	class Str : public std::string {
		//メンバ変数
		EN_CharCode		charCode_;		//文字コード
		std::int32_t	charNum_;		//文字数

	public:
		//コンストラクタ
		Str();
		Str(const std::char8_t* const str, const EN_CharCode charCode);
		Str(const std::string& str, const EN_CharCode charCode);
		//コピーコンストラクタ
		Str(const Str& str);
		//コピー代入演算子
		Str& operator=(const Str& str);
		//デストラクタ
		~Str();
		//文字コード変換
		void convert(const EN_CharCode convCharCode, Str* const convStr) const;
		//文字コードを取得
		EN_CharCode charCode() const { return this->charCode_; }
		//文字数を取得
		std::int32_t charNum() const { return this->charNum_; }

	private:
		//文字数を取得
		std::int32_t getCharNum(const EN_CharCode charCode) const;
		std::int32_t getCharNum_SJIS() const;
		std::int32_t getCharNum_UTF16() const;
		//文字コード変換(SJIS->UTF16)
		void convert_SJIS2UTF16(Str* const utf16, const bool le) const;
	};
}

#endif //INCLUDED_STR_HPP
