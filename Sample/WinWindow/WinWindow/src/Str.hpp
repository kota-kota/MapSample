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

	//文字最大数
	static const std::int32_t MAX_STRNUM = 128;


	/**
	* 1バイト文字クラス
	*/
	class StringU8 {
		//メンバ変数
		EN_CharCode		code_;				//文字コード
		std::int32_t	len_;				//文字列の長さ
		std::int32_t	num_;				//1バイト文字の並びの数
		std::char8_t	str_[MAX_STRNUM];	//1バイト文字の並び

	public:
		//コンストラクタ
		StringU8();
		StringU8(const std::char8_t* const s, const std::int32_t num, const EN_CharCode code);
		//コピーコンストラクタ
		StringU8(const StringU8& str);
		//コピー代入演算子
		StringU8& operator=(const StringU8& str);
		//添え字演算子
		const std::char8_t& operator[](const std::int32_t index) const;
		std::char8_t& operator[](const std::int32_t index);
		//文字追加
		void push_back(const std::char8_t s);
		//文字コードを設定
		void setCode(const EN_CharCode code);
		//文字コードを取得
		EN_CharCode getCode() const;
		//文字列の長さを取得
		std::int32_t getLen() const;
		//1バイト文字の並びの数を取得
		std::int32_t getNum() const;
	};


	/**
	* 2バイト文字クラス
	*/
	class StringU16 {
		//メンバ変数
		EN_CharCode		code_;				//文字コード
		std::int32_t	len_;				//文字列の長さ
		std::int32_t	num_;				//2バイト文字の並びの数
		char16_t		str_[MAX_STRNUM];	//2バイト文字の並び

	public:
		//コンストラクタ
		StringU16();
		StringU16(const char16_t* const s, const std::int32_t num, const EN_CharCode code);
		//コピーコンストラクタ
		StringU16(const StringU16& str);
		//コピー代入演算子
		StringU16& operator=(const StringU16& str);
		//添え字演算子
		const char16_t& operator[](const std::int32_t index) const;
		char16_t& operator[](const std::int32_t index);
		//文字追加
		void push_back(const char16_t s);
		//文字コードを設定
		void setCode(const EN_CharCode code);
		//文字コードを取得
		EN_CharCode getCode() const;
		//文字列の長さを取得
		std::int32_t getLen() const;
		//2バイト文字の並びの数を取得
		std::int32_t getNum() const;
	};


	/**
	* 文字列インターフェースクラス
	*/
	class StringIF {
	public:
		//文字列の長さをカウント(1バイト文字)
		static std::int32_t countLength(const StringU8& str);
		//文字列の長さをカウント(2バイト文字)
		static std::int32_t countLength(const StringU16& wstr);
		//文字コード変換(1バイト文字->1バイト文字)
		static void convert(const StringU8& str, EN_CharCode convCode, StringU8* const convStr);
		//文字コード変換(2バイト文字->2バイト文字)
		static void convert(const StringU16& str, EN_CharCode convCode, StringU16* const convStr);
		//文字コード変換(1バイト文字->2バイト文字)
		static void convert(const StringU8& str, EN_CharCode convCode, StringU16* const convStr);
		//文字コード変換(2バイト文字->1バイト文字)
		static void convert(const StringU16& str, EN_CharCode convCode, StringU8* const convStr);

	private:
		//文字列の長さをカウント(SJIS)
		static std::int32_t countLength_SJIS(const StringU8& sjis);
		//文字列の長さをカウント(UTF16)
		static std::int32_t countLength_UTF16(const StringU16& utf16);
		//文字列の長さをカウント(UTF8)
		static std::int32_t countLength_UTF8(const StringU8& utf8);
		//文字コード変換(SJIS->UTF16)
		static void convert_SJIS_to_UTF16(const StringU8& sjis, StringU16* const utf16, const EN_WCharEndian endian);
		//文字コード変換(SJIS->UTF8)
		static void convert_SJIS_to_UTF8(const StringU8& sjis, StringU8* const utf8);
		//文字コード変換(UTF8->UTF16)
		static void convert_UTF8_to_UTF16(const StringU8& utf8, StringU16* const utf16, const EN_WCharEndian endian);
	};
}

#endif //INCLUDED_STR_HPP
