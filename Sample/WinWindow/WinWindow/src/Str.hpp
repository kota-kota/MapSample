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

	//ワイド文字エンディアン
	enum EN_WCharEndian {
		BIG,
		LITTLE,
	};



	/**
	* マルチバイト文字クラス
	*/
	class String : public std::string {
		//メンバ変数
		EN_CharCode		code_;	//文字コード
		std::int32_t	num_;	//文字数

	public:
		//コンストラクタ
		String();
		String(const std::char8_t* const s, const EN_CharCode code);
		//コピーコンストラクタ
		String(const String& str);
		//コピー代入演算子
		String& operator=(const String& str);
		//文字コードを設定
		void setCode(const EN_CharCode code);
		//文字コードを取得
		EN_CharCode getCode() const;
		//文字数を取得
		std::int32_t getNum() const;
	};


	/**
	* ワイド文字クラス
	*/
	class WString : public std::wstring {
		//メンバ変数
		EN_CharCode		code_;	//文字コード
		std::int32_t	num_;	//文字数

	public:
		//コンストラクタ
		WString();
		WString(const wchar_t* const s, const EN_CharCode code);
		//コピーコンストラクタ
		WString(const WString& str);
		//コピー代入演算子
		WString& operator=(const WString& str);
		//文字コードを設定
		void setCode(const EN_CharCode code);
		//文字コードを取得
		EN_CharCode getCode() const;
		//文字数を取得
		std::int32_t getNum() const;
	};


	/**
	* 文字列インターフェースクラス
	*/
	class StringIF {
	public:
		//文字数カウント(マルチバイト文字)
		static std::int32_t countNum(const String& str);
		//文字数カウント(ワイド文字)
		static std::int32_t countNum(const WString& wstr);
		//文字コード変換(マルチバイト文字->マルチバイト文字)
		static void convert(const String& str, EN_CharCode convCode, String* const convStr);
		//文字コード変換(ワイド文字->ワイド文字)
		static void convert(const WString& wstr, EN_CharCode convCode, WString* const convWStr);
		//文字コード変換(マルチバイト文字->ワイド文字)
		static void convert(const String& str, EN_CharCode convCode, WString* const convWStr);
		//文字コード変換(ワイド文字->マルチバイト文字)
		static void convert(const WString& wstr, EN_CharCode convCode, String* const convStr);

	private:
		//文字数カウント(SJIS)
		static std::int32_t countNum_SJIS(const String& sjis);
		//文字数カウント(UTF16)
		static std::int32_t countNum_UTF16(const WString& utf16);
		//文字数カウント(UTF8)
		static std::int32_t countNum_UTF8(const String& utf8);
		//文字コード変換(SJIS->UTF16)
		static void convert_SJIS_to_UTF16(const String& sjis, WString* const utf16, const EN_WCharEndian endian);
		//文字コード変換(SJIS->UTF8)
		static void convert_SJIS_to_UTF8(const String& sjis, String* const utf8);
		//文字コード変換(UTF8->UTF16)
		static void convert_UTF8_to_UTF16(const String& utf8, WString* const utf16, const EN_WCharEndian endian);
	};
}

#endif //INCLUDED_STR_HPP
