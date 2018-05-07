#include "Str.hpp"
#include "sjis2utf16be.hpp"
#include <memory>



namespace fw {

	/**
	* マルチバイト文字クラス
	*/

	//コンストラクタ
	String::String() :
		std::string(), code_(UNKNOWN), num_(0)
	{
	}
	String::String(const std::char8_t* const s, const EN_CharCode code) :
		std::string(s), code_(UNKNOWN), num_(0)
	{
		//文字コード設定
		this->setCode(code);
	}

	//コピーコンストラクタ
	String::String(const String& str)
	{
		std::string::operator=(str.c_str());
		this->code_ = str.code_;
		this->num_ = str.num_;
	}

	//コピー代入演算子
	String& String::operator=(const String& str)
	{
		std::string::operator=(str.c_str());
		this->code_ = str.code_;
		this->num_ = str.num_;
		return *this;
	}

	//文字コードを設定
	void String::setCode(const EN_CharCode code)
	{
		if (this->code_ != code) {
			//文字コードを設定し、文字コードに応じて文字数をカウント
			this->code_ = code;
			this->num_ = StringIF::countNum(*this);
		}
		else {
			//文字コードが同じ場合は何もしない
		}
	}

	//文字コードを取得
	EN_CharCode String::getCode() const
	{
		return this->code_;
	}

	//文字数を取得
	std::int32_t String::getNum() const
	{
		return this->num_;
	}


	/**
	* ワイド文字クラス
	*/

	//コンストラクタ
	WString::WString() :
		std::wstring(), code_(UNKNOWN), num_(0)
	{
	}
	WString::WString(const wchar_t* const s, const EN_CharCode code) :
		std::wstring(s), code_(UNKNOWN), num_(0)
	{
		//文字コード設定
		this->setCode(code);
	}

	//コピーコンストラクタ
	WString::WString(const WString& str)
	{
		std::wstring::operator=(str.c_str());
		this->code_ = str.code_;
	}

	//コピー代入演算子
	WString& WString::operator=(const WString& str)
	{
		std::wstring::operator=(str.c_str());
		this->code_ = str.code_;
		return *this;
	}

	//文字コードを設定
	void WString::setCode(const EN_CharCode code)
	{
		if (this->code_ != code) {
			//文字コードを設定し、文字コードに応じて文字数をカウント
			this->code_ = code;
			this->num_ = StringIF::countNum(*this);
		}
		else {
			//文字コードが同じ場合は何もしない
		}
	}

	//文字コードを取得
	EN_CharCode WString::getCode() const
	{
		return this->code_;
	}

	//文字数を取得
	std::int32_t WString::getNum() const
	{
		return this->num_;
	}



	/**
	* 文字列インターフェースクラス
	*/

	//文字数カウント(マルチバイト文字)
	std::int32_t StringIF::countNum(const String& str)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		const EN_CharCode code = str.getCode();
		switch (code) {
		case SJIS:
			retNum = StringIF::countNum_SJIS(str);
			break;
		case UTF8:
			retNum = StringIF::countNum_UTF8(str);
			break;
		default:
			//未サポートまたはワイド文字
			break;
		};

		return retNum;
	}
	//文字数カウント(ワイド文字)
	std::int32_t StringIF::countNum(const WString& wstr)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		const EN_CharCode code = wstr.getCode();
		switch (code) {
		case UTF16BE:
		case UTF16LE:
			retNum = StringIF::countNum_UTF16(wstr);
			break;
		default:
			//未サポートまたはマルチバイト文字
			break;
		};

		return retNum;
	}

	//文字コード変換(マルチバイト文字->マルチバイト文字)
	void StringIF::convert(const String& str, EN_CharCode convCode, String* const convStr)
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
	//文字コード変換(ワイド文字->ワイド文字)
	void StringIF::convert(const WString& wstr, EN_CharCode convCode, WString* const convWStr)
	{
		const EN_CharCode code = wstr.getCode();
		if (code == convCode) {
			//同じ文字コードの場合はそのままコピー
			*convWStr = wstr;
		}
		else {
			//違う文字コードの場合は変換
		}
	}
	//文字コード変換(マルチバイト文字->ワイド文字)
	void StringIF::convert(const String& str, EN_CharCode convCode, WString* const convWStr)
	{
		const EN_CharCode code = str.getCode();
		if (code == SJIS) {
			if (convCode == UTF16BE) {
				//文字コード変換(SJIS->UTF16)
				StringIF::convert_SJIS_to_UTF16(str, convWStr, BIG);
			}
			else if (convCode == UTF16LE) {
				//文字コード変換(SJIS->UTF16)
				StringIF::convert_SJIS_to_UTF16(str, convWStr, LITTLE);
			}
			else {
			}
		}
		else if (code == UTF8) {
			if (convCode == UTF16BE) {
				//文字コード変換(UTF8->UTF16)
				StringIF::convert_UTF8_to_UTF16(str, convWStr, BIG);
			}
			else if (convCode == UTF16LE) {
				//文字コード変換(UTF8->UTF16)
				StringIF::convert_UTF8_to_UTF16(str, convWStr, LITTLE);
			}
			else {
			}
		}
		else {
		}
	}
	//文字コード変換(ワイド文字->マルチバイト文字)
	void StringIF::convert(const WString& wstr, EN_CharCode convCode, String* const convStr)
	{
		const EN_CharCode code = wstr.getCode();
		if (code == UTF16BE) {
			if (convCode == SJIS) {
				convStr;
			}
		}
		else {
		}
	}

	//文字数カウント(SJIS)
	std::int32_t StringIF::countNum_SJIS(const String& sjis)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		//対象文字サイズ
		const std::size_t size = sjis.size();

		std::size_t readSize = 0;
		while (readSize < size) {
			const std::uint8_t c = std::uint8_t(sjis[readSize]);

			//文字数加算
			retNum++;

			if (((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC))) {
				//2バイト文字
				readSize += 2;
			}
			else {
				//1バイト文字
				readSize += 1;
			}
		}

		return retNum;
	}

	//文字数カウント(UTF16)
	std::int32_t StringIF::countNum_UTF16(const WString& utf16)
	{
		//対象文字サイズが文字数(出力)
		return static_cast<std::int32_t>(utf16.size());
	}

	//文字数カウント(UTF8)
	std::int32_t StringIF::countNum_UTF8(const String& utf8)
	{
		//文字数(出力)
		std::int32_t retNum = 0;

		//対象文字サイズ
		const std::size_t size = utf8.size();

		std::size_t readSize = 0;
		while (readSize < size) {
			const std::uint8_t c = std::uint8_t(utf8[readSize]);

			//文字数加算
			retNum++;

			if (c <= 0x7F) {
				//1バイト文字
				readSize += 1;
			}
			else if (c <= 0xDF) {
				//2バイト文字
				readSize += 2;
			}
			else {
				//3バイト文字
				readSize += 3;
			}
		}

		return retNum;
	}

	//文字コード変換(SJIS->UTF16)
	void StringIF::convert_SJIS_to_UTF16(const String& sjis, WString* const utf16, const EN_WCharEndian endian)
	{
		//変換前(SJIS)の文字情報
		const std::int32_t num = sjis.getNum();
		const std::size_t sjisSize = sjis.size();

#if 1
		//デバッグ用
		for (int32_t c = 0; c < sjisSize; c++) {
			printf("0x%x,", std::uint8_t(sjis[c]));
		}
		printf("\n");
#endif

		//変換後(UTF16)は文字数*2バイト分なので事前に領域を確保
		utf16->reserve(num * 2);

		//変換
		std::size_t readSize = 0;
		while (readSize < sjisSize) {
			std::uint16_t s = 0x0000;
			std::uint16_t u = 0x0000;

			//SJIS1バイト目
			const std::uint8_t s1 = static_cast<std::uint8_t>(sjis[readSize]);
			if ((s1 >= 0x81) && (s1 <= 0x9F)) {
				//SJIS全角文字の1バイト目なら2バイト文字

				//SJIS2バイト目を取得してSJIS全角文字コードに変換
				const std::uint8_t s2 = static_cast<std::uint8_t>(sjis[readSize + 1]);
				s = (static_cast<std::uint16_t>(s1) << 8) | static_cast<std::uint16_t>(s2);

				//SJIS->UTF16LE変換テーブルを使用して変換
				//0x8100がテーブルの0番目となる
				u = sjis2utf16be_8100_9FFF[s - 0x8100];

				readSize += 2;
			}
			else if ((s1 >= 0xE0) && (s1 <= 0xFC)) {
				//SJIS全角文字の1バイト目なら2バイト文字

				//SJIS2バイト目を取得してSJIS全角文字コードに変換
				const std::uint8_t s2 = static_cast<std::uint8_t>(sjis[readSize + 1]);
				s = (static_cast<std::uint16_t>(s1) << 8) | static_cast<std::uint16_t>(s2);

				//SJIS->UTF16LE変換テーブルを使用して変換
				//0xE000がテーブルの0番目となる
				u = sjis2utf16be_E000_FCFF[s - 0xE000];

				readSize += 2;
			}
			else {
				//SJIS半角文字なら1バイト文字

				//SJIS1バイト目がSJIS半角文字コード
				s = s1;

				//SJIS->UTF16LE変換テーブルを使用して変換
				u = sjis2utf16be_0000_00FF[s];

				readSize += 1;
			}

			if (u == 0x0000) {
				printf("[ERROR] %s:%d SJIS:0x%04x UTF16BE:0x%04x\n", __FUNCTION__, __LINE__, s, u);
			}
			else {
				printf("SJIS:0x%04x UTF16BE:0x%04x\n", s, u);
			}

			//UTF16を1バイトに分割して格納
			if (endian == LITTLE) {
				//リトルエンディアン
				const std::uint8_t u1 = static_cast<std::uint8_t>((u & 0xFF00) >> 8);
				const std::uint8_t u2 = static_cast<std::uint8_t>(u & 0x00FF);
				u = (static_cast<std::uint16_t>(u2) << 8) | static_cast<std::uint16_t>(u1);
				utf16->push_back(static_cast<wchar_t>(u));
			}
			else {
				//ビッグエンディアン
				utf16->push_back(static_cast<wchar_t>(u));
			}
		}

		//変換後(UTF16)の文字コードを設定(文字数をカウントするため、ここで設定)
		const EN_CharCode code = (endian == LITTLE) ? UTF16LE : UTF16BE;
		utf16->setCode(code);
	}

	//文字コード変換(SJIS->UTF8)
	void StringIF::convert_SJIS_to_UTF8(const String& sjis, String* const utf8)
	{
		//変換前(SJIS)の文字情報
		const std::int32_t num = sjis.getNum();

		//まずはUTF16BEに変換
		WString utf16be;
		StringIF::convert_SJIS_to_UTF16(sjis, &utf16be, BIG);

		//変換後(UTF8)は文字数*3バイト分が最大となるので事前に領域を確保
		utf8->reserve(num * 2);

		//文字数分ループ
		const std::int32_t numUtf16 = utf16be.getNum();
		for (std::int32_t i = 0; i < numUtf16; i++) {
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
	void StringIF::convert_UTF8_to_UTF16(const String& utf8, WString* const utf16, const EN_WCharEndian endian)
	{
		//変換前(UTF8)の文字情報
		const std::int32_t num = utf8.getNum();
		const std::size_t utf8Size = utf8.size();

		//変換後(UTF16)は文字数*2バイト分なので事前に領域を確保
		utf16->reserve(num * 2);

		//変換
		std::size_t readSize = 0;
		while (readSize < utf8Size) {
			const std::uint8_t c1 = static_cast<std::uint8_t>(utf8[readSize]);

			std::uint16_t u = 0x0000;
			if (c1 <= 0x7F) {
				//1バイト文字
				u = static_cast<std::uint16_t>(c1);

				readSize += 1;
			}
			else if (c1 <= 0xDF) {
				//2バイト文字
				const std::uint8_t c2 = static_cast<std::uint8_t>(utf8[readSize + 1]);
				u |= static_cast<std::uint16_t>((c1 & 0x1F) << 6);
				u |= static_cast<std::uint16_t>(c2 & 0x3F);

				readSize += 2;
			}
			else {
				//3バイト文字
				const std::uint8_t c2 = static_cast<std::uint8_t>(utf8[readSize + 1]);
				const std::uint8_t c3 = static_cast<std::uint8_t>(utf8[readSize + 2]);
				u |= static_cast<std::uint16_t>((c1 & 0x0F) << 12);
				u |= static_cast<std::uint16_t>((c2 & 0x3F) << 6);
				u |= static_cast<std::uint16_t>(c3 & 0x3F);

				readSize += 3;
			}

			utf16->push_back(static_cast<wchar_t>(u));
		}

		//変換後(UTF16)の文字コードを設定(文字数をカウントするため、ここで設定)
		const EN_CharCode code = (endian == LITTLE) ? UTF16LE : UTF16BE;
		utf16->setCode(code);
	}
}
