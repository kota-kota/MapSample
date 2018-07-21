#include "Str.hpp"
#include "StrCodeTable_sjis2utf16be.hpp"
#include <memory>



namespace fw {

	/**
	* 1バイト文字クラス
	*/

	//コンストラクタ
	StringU8::StringU8() :
		code_(UNKNOWN), len_(0), num_(0), str_()
	{
		memset(&this->str_[0], '\0', MAX_STRNUM);
	}
	StringU8::StringU8(const std::char8_t* const s, const std::int32_t num, const EN_CharCode code) :
		code_(UNKNOWN), len_(0), num_(num), str_()
	{
		//文字列を保持
		memset(&this->str_[0], '\0', MAX_STRNUM);
		(void)memcpy_s(&this->str_[0], MAX_STRNUM, s, num);

		//文字コード設定
		this->setCode(code);
	}

	//コピーコンストラクタ
	StringU8::StringU8(const StringU8& str)
	{
		this->code_ = str.code_;
		this->len_ = str.len_;
		this->num_ = str.num_;
		(void)memcpy_s(&this->str_[0], this->num_, &str.str_[0], str.num_);
	}

	//コピー代入演算子
	StringU8& StringU8::operator=(const StringU8& str)
	{
		this->code_ = str.code_;
		this->len_ = str.len_;
		this->num_ = str.num_;
		(void)memcpy_s(&this->str_[0], this->num_, &str.str_[0], str.num_);
		return *this;
	}

	//添え字演算子
	const std::char8_t& StringU8::operator[](const std::int32_t index) const
	{
		return this->str_[index];
	}
	std::char8_t& StringU8::operator[](const std::int32_t index)
	{
		return this->str_[index];
	}

	//文字追加
	void StringU8::push_back(const std::char8_t s)
	{
		this->str_[this->num_] = s;
		this->num_++;
	}

	//文字コードを設定
	void StringU8::setCode(const EN_CharCode code)
	{
		if (this->code_ != code) {
			//文字コードを設定し、文字コードに応じて文字列の長さをカウント
			this->code_ = code;
			this->len_ = StringIF::countLength(*this);
		}
		else {
			//文字コードが同じ場合は何もしない
		}
	}

	//文字コードを取得
	EN_CharCode StringU8::getCode() const
	{
		return this->code_;
	}

	//文字列の長さを取得
	std::int32_t StringU8::getLen() const
	{
		return this->len_;
	}

	//1バイト文字の並びの数を取得
	std::int32_t StringU8::getNum() const
	{
		return this->num_;
	}


	/**
	* 2バイト文字クラス
	*/

	//コンストラクタ
	StringU16::StringU16() :
		code_(UNKNOWN), len_(0), num_(0), str_()
	{
		memset(&this->str_[0], '\0', MAX_STRNUM * sizeof(char16_t));
	}
	StringU16::StringU16(const char16_t* const s, const std::int32_t num, const EN_CharCode code) :
		code_(UNKNOWN), len_(0), num_(num), str_()
	{
		//文字列を保持
		memset(&this->str_[0], '\0', MAX_STRNUM * sizeof(char16_t));
		(void)memcpy_s(&this->str_[0], MAX_STRNUM * sizeof(char16_t), s, num * sizeof(char16_t));

		//文字コード設定
		this->setCode(code);
	}

	//コピーコンストラクタ
	StringU16::StringU16(const StringU16& str)
	{
		this->code_ = str.code_;
		this->len_ = str.len_;
		this->num_ = str.num_;
		(void)memcpy_s(&this->str_[0], this->num_ * sizeof(char16_t), &str.str_[0], str.num_ * sizeof(char16_t));
	}

	//コピー代入演算子
	StringU16& StringU16::operator=(const StringU16& str)
	{
		this->code_ = str.code_;
		this->len_ = str.len_;
		this->num_ = str.num_;
		(void)memcpy_s(&this->str_[0], this->num_ * sizeof(char16_t), &str.str_[0], str.num_ * sizeof(char16_t));
		return *this;
	}

	//添え字演算子
	const char16_t& StringU16::operator[](const std::int32_t index) const
	{
		return this->str_[index];
	}
	char16_t& StringU16::operator[](const std::int32_t index)
	{
		return this->str_[index];
	}

	//文字追加
	void StringU16::push_back(const char16_t s)
	{
		this->str_[this->num_] = s;
		this->num_++;
	}

	//文字コードを設定
	void StringU16::setCode(const EN_CharCode code)
	{
		if (this->code_ != code) {
			//文字コードを設定し、文字コードに応じて文字数をカウント
			this->code_ = code;
			this->len_ = StringIF::countLength(*this);
		}
		else {
			//文字コードが同じ場合は何もしない
		}
	}

	//文字コードを取得
	EN_CharCode StringU16::getCode() const
	{
		return this->code_;
	}

	//文字列の長さを取得
	std::int32_t StringU16::getLen() const
	{
		return this->len_;
	}

	//2バイト文字の並びの数を取得
	std::int32_t StringU16::getNum() const
	{
		return this->num_;
	}



	/**
	* 文字列インターフェースクラス
	*/

	//文字列の長さをカウント(1バイト文字)
	std::int32_t StringIF::countLength(const StringU8& str)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		//文字コード毎の処理
		const EN_CharCode code = str.getCode();
		switch (code) {
		case SJIS:
			retNum = StringIF::countLength_SJIS(str);
			break;
		case UTF8:
			retNum = StringIF::countLength_UTF8(str);
			break;
		default:
			//未サポートまたはワイド文字
			break;
		};

		return retNum;
	}
	//文字列の長さをカウント(2バイト文字)
	std::int32_t StringIF::countLength(const StringU16& str)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		//文字コード毎の処理
		const EN_CharCode code = str.getCode();
		switch (code) {
		case UTF16BE:
		case UTF16LE:
			retNum = StringIF::countLength_UTF16(str);
			break;
		default:
			//未サポートまたはマルチバイト文字
			break;
		};

		return retNum;
	}

	//文字コード変換(1バイト文字->1バイト文字)
	void StringIF::convert(const StringU8& str, EN_CharCode convCode, StringU8* const convStr)
	{
		const EN_CharCode code = str.getCode();
		if (code == convCode) {
			//同じ文字コードの場合はそのままコピー
			*convStr = str;
		}
		else {
			//違う文字コードの場合は変換
			if (code == SJIS) {
				if (convCode == UTF8) {
					//文字コード変換(SJIS->UTF8)
					StringIF::convert_SJIS_to_UTF8(str, convStr);
				}
				else {
				}
			}
			else {
			}
		}
	}
	//文字コード変換(2バイト文字->2バイト文字)
	void StringIF::convert(const StringU16& str, EN_CharCode convCode, StringU16* const convStr)
	{
		const EN_CharCode code = str.getCode();
		if (code == convCode) {
			//同じ文字コードの場合はそのままコピー
			*convStr = str;
		}
		else {
			//違う文字コードの場合は変換
		}
	}
	//文字コード変換(1バイト文字->2バイト文字)
	void StringIF::convert(const StringU8& str, EN_CharCode convCode, StringU16* const convStr)
	{
		const EN_CharCode code = str.getCode();
		if (code == SJIS) {
			if (convCode == UTF16BE) {
				//文字コード変換(SJIS->UTF16)
				StringIF::convert_SJIS_to_UTF16(str, convStr, BIG);
			}
			else if (convCode == UTF16LE) {
				//文字コード変換(SJIS->UTF16)
				StringIF::convert_SJIS_to_UTF16(str, convStr, LITTLE);
			}
			else {
			}
		}
		else if (code == UTF8) {
			if (convCode == UTF16BE) {
				//文字コード変換(UTF8->UTF16)
				StringIF::convert_UTF8_to_UTF16(str, convStr, BIG);
			}
			else if (convCode == UTF16LE) {
				//文字コード変換(UTF8->UTF16)
				StringIF::convert_UTF8_to_UTF16(str, convStr, LITTLE);
			}
			else {
			}
		}
		else {
		}
	}
	//文字コード変換(2バイト文字->1バイト文字)
	void StringIF::convert(const StringU16& str, EN_CharCode convCode, StringU8* const convStr)
	{
		const EN_CharCode code = str.getCode();
		if (code == UTF16BE) {
			if (convCode == SJIS) {
				convStr;
			}
		}
		else {
		}
	}

	//文字列の長さをカウント(SJIS)
	std::int32_t StringIF::countLength_SJIS(const StringU8& sjis)
	{
		//文字列の長さ(出力)
		std::int32_t retLen = 0;

		//1バイト文字の並びの数
		const std::int32_t num = sjis.getNum();

		std::int32_t readNum = 0;
		while (readNum < num) {
			const std::uint8_t c = std::uint8_t(sjis[readNum]);

			//文字列の長さを加算
			retLen++;

			if (((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC))) {
				//2バイト文字
				readNum += 2;
			}
			else {
				//1バイト文字
				readNum += 1;
			}
		}

		return retLen;
	}

	//文字列の長さをカウント(UTF16)
	std::int32_t StringIF::countLength_UTF16(const StringU16& utf16)
	{
		//2バイト文字の並びの数が文字数(出力)
		return utf16.getNum();
	}

	//文字列の長さをカウント(UTF8)
	std::int32_t StringIF::countLength_UTF8(const StringU8& utf8)
	{
		//文字列の長さ(出力)
		std::int32_t retLen = 0;

		//1バイト文字の並びの数
		const std::int32_t num = utf8.getNum();

		std::int32_t readNum = 0;
		while (readNum < num) {
			const std::uint8_t c = std::uint8_t(utf8[readNum]);

			//文字列の長さを加算
			retLen++;

			if (c <= 0x7F) {
				//1バイト文字
				readNum += 1;
			}
			else if (c <= 0xDF) {
				//2バイト文字
				readNum += 2;
			}
			else {
				//3バイト文字
				readNum += 3;
			}
		}

		return retLen;
	}

	//文字コード変換(SJIS->UTF16)
	void StringIF::convert_SJIS_to_UTF16(const StringU8& sjis, StringU16* const utf16, const EN_WCharEndian endian)
	{
		//変換前(SJIS)の1バイト文字の並びの数を取得
		const std::int32_t sjisNum = sjis.getNum();

		//変換後(UTF16)の文字の並びの数
		std::int32_t utf16Num = 0;

		//変換
		std::int32_t readNum = 0;
		while (readNum < sjisNum) {
			std::uint16_t s = 0x0000;
			std::uint16_t u = 0x0000;

			//SJIS1バイト目
			const std::uint8_t s1 = static_cast<std::uint8_t>(sjis[readNum]);
			if ((s1 >= 0x81) && (s1 <= 0x9F)) {
				//SJIS全角文字の1バイト目なら2バイト文字

				//SJIS2バイト目を取得してSJIS全角文字コードに変換
				const std::uint8_t s2 = static_cast<std::uint8_t>(sjis[readNum + 1]);
				s = (static_cast<std::uint16_t>(s1) << 8) | static_cast<std::uint16_t>(s2);

				//SJIS->UTF16LE変換テーブルを使用して変換
				//0x8100がテーブルの0番目となる
				u = sjis2utf16be_8100_9FFF[s - 0x8100];

				readNum += 2;
			}
			else if ((s1 >= 0xE0) && (s1 <= 0xFC)) {
				//SJIS全角文字の1バイト目なら2バイト文字

				//SJIS2バイト目を取得してSJIS全角文字コードに変換
				const std::uint8_t s2 = static_cast<std::uint8_t>(sjis[readNum + 1]);
				s = (static_cast<std::uint16_t>(s1) << 8) | static_cast<std::uint16_t>(s2);

				//SJIS->UTF16LE変換テーブルを使用して変換
				//0xE000がテーブルの0番目となる
				u = sjis2utf16be_E000_FCFF[s - 0xE000];

				readNum += 2;
			}
			else {
				//SJIS半角文字なら1バイト文字

				//SJIS1バイト目がSJIS半角文字コード
				s = s1;

				//SJIS->UTF16LE変換テーブルを使用して変換
				u = sjis2utf16be_0000_00FF[s];

				readNum += 1;
			}

			if (u == 0x0000) {
				printf("[ERROR] %s:%d SJIS:0x%04x UTF16BE:0x%04x\n", __FUNCTION__, __LINE__, s, u);
				continue;
			}

			//UTF16を1バイトに分割して格納
			if (endian == LITTLE) {
				//リトルエンディアン
				const std::uint8_t u1 = static_cast<std::uint8_t>((u & 0xFF00) >> 8);
				const std::uint8_t u2 = static_cast<std::uint8_t>(u & 0x00FF);
				u = (static_cast<std::uint16_t>(u2) << 8) | static_cast<std::uint16_t>(u1);
				utf16->push_back(static_cast<char16_t>(u));
			}
			else {
				//ビッグエンディアン
				utf16->push_back(static_cast<char16_t>(u));
			}
		}

		//変換後(UTF16)の文字コードを設定(文字数をカウントするため、ここで設定)
		const EN_CharCode code = (endian == LITTLE) ? UTF16LE : UTF16BE;
		utf16->setCode(code);
	}

	//文字コード変換(SJIS->UTF8)
	void StringIF::convert_SJIS_to_UTF8(const StringU8& sjis, StringU8* const utf8)
	{
		//まずはUTF16BEに変換
		StringU16 utf16be;
		StringIF::convert_SJIS_to_UTF16(sjis, &utf16be, BIG);

		//文字数分ループ
		const std::int32_t utf16Num = utf16be.getNum();
		for (std::int32_t i = 0; i < utf16Num; i++) {
			const std::uint16_t u = static_cast<std::uint16_t>(utf16be[i]);

			//1バイト毎に分割
			const std::uint8_t u1 = static_cast<std::uint8_t>((u & 0xFF00) >> 8);
			const std::uint8_t u2 = static_cast<std::uint8_t>(u & 0x00FF);

			const std::uint16_t c = (u1 * 0x100) + u2;
			if (c <= 0x007F) {
				//0x0000-0x007F
				const std::char8_t c1 = static_cast<std::char8_t>(c);
				utf8->push_back(c1);
			}
			else if (c <= 0x07FF) {
				//0x0080-0x07FF
				const std::char8_t c1 = static_cast<std::char8_t>(0xC0 | (c >> 6));
				const std::char8_t c2 = static_cast<std::char8_t>(0x80 | (c & 0x003F));
				utf8->push_back(c1);
				utf8->push_back(c2);
			}
			else {
				//0x800-0xFFFF
				const std::char8_t c1 = static_cast<std::char8_t>(0xE0 | ((c >> 12) & 0x000F));
				const std::char8_t c2 = static_cast<std::char8_t>(0x80 | ((c >> 6) & 0x003F));
				const std::char8_t c3 = static_cast<std::char8_t>(0x80 | (c & 0x003F));
				utf8->push_back(c1);
				utf8->push_back(c2);
				utf8->push_back(c3);
			}
		}

		//変換後(UTF8)の文字コードを設定(文字数をカウントするため、ここで設定)
		utf8->setCode(UTF8);
	}

	//文字コード変換(UTF8->UTF16)
	void StringIF::convert_UTF8_to_UTF16(const StringU8& utf8, StringU16* const utf16, const EN_WCharEndian endian)
	{
		//変換前(UTF8)の文字情報1バイト文字の並びの数を取得
		const std::int32_t utf8Num = utf8.getNum();

		//変換
		std::int32_t readNum = 0;
		while (readNum < utf8Num) {
			const std::uint8_t c1 = static_cast<std::uint8_t>(utf8[readNum]);

			std::uint16_t u = 0x0000;
			if (c1 <= 0x7F) {
				//1バイト文字
				u = static_cast<std::uint16_t>(c1);

				readNum += 1;
			}
			else if (c1 <= 0xDF) {
				//2バイト文字
				const std::uint8_t c2 = static_cast<std::uint8_t>(utf8[readNum + 1]);
				u |= static_cast<std::uint16_t>((c1 & 0x1F) << 6);
				u |= static_cast<std::uint16_t>(c2 & 0x3F);

				readNum += 2;
			}
			else {
				//3バイト文字
				const std::uint8_t c2 = static_cast<std::uint8_t>(utf8[readNum + 1]);
				const std::uint8_t c3 = static_cast<std::uint8_t>(utf8[readNum + 2]);
				u |= static_cast<std::uint16_t>((c1 & 0x0F) << 12);
				u |= static_cast<std::uint16_t>((c2 & 0x3F) << 6);
				u |= static_cast<std::uint16_t>(c3 & 0x3F);

				readNum += 3;
			}

			utf16->push_back(static_cast<char16_t>(u));
		}

		//変換後(UTF16)の文字コードを設定(文字数をカウントするため、ここで設定)
		const EN_CharCode code = (endian == LITTLE) ? UTF16LE : UTF16BE;
		utf16->setCode(code);
	}
}
