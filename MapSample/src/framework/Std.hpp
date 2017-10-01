#ifndef INCLUDED_STD_HPP
#define INCLUDED_STD_HPP

#include <cstdint>
#include <memory>

namespace std
{
	using char_t = char;
	using float_t = float;
	using double_t = double;
	using byte_t = uint8_t;

	//----------------------------------------------------------
	//
	// ヒープクラス(テンプレート)
	//
	//----------------------------------------------------------
	template <typename T>
	class Heap {
		T*			p_;
		int32_t		num_;

	public:
		//コンストラクタ
		Heap() :
			p_(nullptr), num_(0)
		{
		}
		Heap(int32_t num) :
			p_(nullptr), num_(num)
		{
			p_ = new T[num_];
		}

		//デストラクタ
		~Heap()
		{
			if (p_ != nullptr) {
				delete[] p_;
			}
		}

		//コピーコンストラクタ
		Heap(const Heap& org) :
			p_(nullptr), num_(org.num_)
		{
			this->copy(org);
		}

		//代入演算子オーバーロード
		Heap& operator=(const Heap& org)
		{
			//自身の代入をチェック
			if (this != &org) {
				this->copy(org);
			}
			return *this;
		}

		//添え字演算子オーバーロード
		T operator[](int32_t index) const
		{
			return *(heap_ + index);
		}
		T& operator[](int32_t index)
		{
			return *(heap_ + index);
		}

		operator T*() const
		{
			return heap_;
		}

		//ヒープ確保数を取得
		std::int32_t getNum() const
		{
			return num_;
		}

		//ヒープ確保サイズを取得
		size_t getSize() const
		{
			return sizeof(T) * num_;
		}

	private:
		//コピー
		void copy(const Heap& org)
		{
			if (org.p_ != nullptr) {
				this->p_ = new T[this->num_];
				(void)memcpy_s(this->p_, this->num_, org.heap_, org.num_);
			}
		}
	};


	//----------------------------------------------------------
	//
	// バイナリ操作メソッド群
	//
	//----------------------------------------------------------

	class ByteWriter {
	public:
		//1バイトを書き込み(LE)
		static void write1ByteLe(byte_t* const data, const uint8_t writeData);
		//2バイトを書き込み(LE)
		static void write2ByteLe(byte_t* const data, const uint16_t writeData);
		//4バイトを書き込み(LE)
		static void write4ByteLe(byte_t* const data, const uint32_t writeData);
	};

	class ByteReader {
	public:
		//1バイトを読み込み(LE)
		static void read1ByteLe(const byte_t* const data, uint8_t* const readData);
		//2バイトを読み込み(LE)
		static void read2ByteLe(const byte_t* const data, uint16_t* const readData);
		//4バイトを読み込み(LE)
		static void read4ByteLe(const byte_t* const data, uint32_t* const readData);
	};


	//----------------------------------------------------------
	//
	// 共通構造体定義
	//
	//----------------------------------------------------------

	struct CoordI {
		int32_t		x;
		int32_t		y;
		int32_t		z;
	};

	struct CoordD {
		double_t	x;
		double_t	y;
		double_t	z;
	};

	struct CoordTex {
		double_t	u;
		double_t	v;
		int32_t		x;
		int32_t		y;
		int32_t		z;
	};

	struct Color {
		uint8_t		r;
		uint8_t		g;
		uint8_t		b;
		uint8_t		a;
	};

	struct Size {
		int32_t		width;
		int32_t		height;
	};

	struct ScreenArea {
		int32_t		left;
		int32_t		top;
		int32_t		right;
		int32_t		bottom;
	};

	struct MapArea {
		int32_t		xmin;
		int32_t		ymin;
		int32_t		xmax;
		int32_t		ymax;
	};
}

#endif //INCLUDED_FWSTD_HPP
