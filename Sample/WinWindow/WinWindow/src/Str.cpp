#include "Str.hpp"
#include "sjis2utf16be.hpp"
#include <memory>



namespace fw {

	/**
	* 文字列クラス
	*/

	//コンストラクタ
	Str::Str() :
		std::string(), charCode_(UNKNOWN), charNum_(0)
	{
	}
	Str::Str(const std::char8_t* const pstr, const EN_CharCode charCode) :
		std::string(pstr), charCode_(charCode), charNum_(0)
	{
		//文字数を取得
		this->charNum_ = this->getCharNum(charCode);
	}
	Str::Str(const std::string& str, const EN_CharCode charCode) :
		std::string(str), charCode_(charCode), charNum_(0)
	{
		//文字数を取得
		this->charNum_ = this->getCharNum(charCode);
	}

	//コピーコンストラクタ
	Str::Str(const Str& str)
	{
		std::string::operator=(str.c_str());
		this->charCode_ = str.charCode_;
		this->charNum_ = str.charNum_;
	}

	//コピー代入演算子
	Str& Str::operator=(const Str& str)
	{
		std::string::operator=(str.c_str());
		this->charCode_ = str.charCode_;
		this->charNum_ = str.charNum_;
		return *this;
	}

	//デストラクタ
	Str::~Str()
	{
	}

	//文字コード変換
	void Str::convert(const EN_CharCode convCharCode, Str* const convStr) const
	{
		if(this->charCode_ == SJIS) {
			if (convCharCode == UTF16LE) {
				//SJISからUTF16LEへ変換
				this->convert_SJIS2UTF16(convStr, true);
			}
			else if (convCharCode == UTF16BE) {
				//SJISからUTF16BEへ変換
				this->convert_SJIS2UTF16(convStr, false);
			}
			else {
			}
		}
		else {
		}
	}


	//文字数を取得
	std::int32_t Str::getCharNum(const EN_CharCode charCode) const
	{
		std::int32_t charNum = 0;

		switch (charCode) {
		case EN_CharCode::SJIS:		//SJIS
			charNum = this->getCharNum_SJIS();
			break;
		case EN_CharCode::UTF16BE:	//UTF16(BE)
		case EN_CharCode::UTF16LE:	//UTF16(LE)
			charNum = this->getCharNum_UTF16();
			break;
		default:
			break;
		}

		return charNum;
	}
	//文字数を取得(SJIS)
	std::int32_t Str::getCharNum_SJIS() const
	{
		//対象の文字データ
		const std::string& str = *this;
		const std::size_t size = str.size();

		//文字数(出力)
		std::int32_t charNum = 0;

		std::size_t readSize = 0;
		while (readSize < size) {
			const std::uint8_t c = std::uint8_t(str[readSize]);

			//文字数加算
			charNum++;

			if (((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC))) {
				//SJIS全角文字の1バイト目なら2バイト文字
				readSize += 2;
			}
			else {
				//SJIS半角文字なら1バイト文字
				readSize += 1;
			}
		}

		return charNum;
	}
	//文字数を取得(UTF16)
	std::int32_t Str::getCharNum_UTF16() const
	{
		//対象の文字データ
		const std::string& str = *this;
		const std::size_t size = str.size();

		//文字数(出力)
		return size / 2;
	}

	//文字コード変換(SJIS->UTF16)
	void Str::convert_SJIS2UTF16(Str* const utf16, const bool le) const
	{
		//変換後(UTF16)の文字列情報を格納
		utf16->reserve(this->charNum_ * 2);	//UTF16は文字数*2バイト分なので事前に領域を確保
		utf16->charNum_ = this->charNum_;

		//変換前(SJIS)の文字列情報データ
		const std::string& sjis = *this;
		const std::size_t sjisSize = sjis.size();

#if 1
		//デバッグ用
		for (int32_t c = 0; c < sjisSize; c++) {
			printf("0x%x,", std::uint8_t(sjis[c]));
		}
		printf("\n");
#endif

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
				printf("[ERROR] %s:%d SJIS:0x%04x UTF16LE:0x%04x\n", __FUNCTION__, __LINE__, s, u);
			}
			else {
				printf("SJIS:0x%04x UTF16LE:0x%04x\n", s, u);
			}

			//UTF16を1バイトに分割して格納
			if (le) {
				//リトルエンディアン
				utf16->push_back(static_cast<std::char8_t>(u & 0x00FF));
				utf16->push_back(static_cast<std::char8_t>((u & 0xFF00) >> 8));
			}
			else {
				//ビッグエンディアン
				utf16->push_back(static_cast<std::char8_t>((u & 0xFF00) >> 8));
				utf16->push_back(static_cast<std::char8_t>(u & 0x00FF));
			}
		}

		if (le) {
			utf16->charCode_ = EN_CharCode::UTF16LE;
		}
		else {
			utf16->charCode_ = EN_CharCode::UTF16BE;
		}
	}
}
