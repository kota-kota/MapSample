#ifndef INCLUDED_FWSTD_HPP
#define INCLUDED_FWSTD_HPP

#include <cstdint>
#include <memory>

namespace std
{
	using char_t = char;
	using float32_t = float;
	using float64_t = double;
}

namespace fw
{
	//----------------------------------------------------------
	//
	// ヒープメモリクラス(テンプレート)
	//
	//----------------------------------------------------------
	template <typename T>
	class HeapMemory {
		T*				heap_;
		std::int32_t	num_;

	public:
		//コンストラクタ
		HeapMemory() :
			heap_(nullptr), num_(0)
		{
		}
		HeapMemory(std::int32_t num) :
			heap_(nullptr), num_(num)
		{
			heap_ = new T[num_];
		}

		//デストラクタ
		~HeapMemory()
		{
			if (heap_ != nullptr) {
				delete[] heap_;
			}
		}

		//コピーコンストラクタ
		HeapMemory(const HeapMemory& org) :
			heap_(nullptr), num_(org.num_)
		{
			if (org.heap_ != nullptr) {
				heap_ = new T[num_];
				(void)memcpy_s(heap_, num_, org.heap_, org.num_);
			}
		}

		//代入演算子オーバーロード
		HeapMemory& operator=(const HeapMemory& org)
		{
			//自身の代入をチェック
			if (this != &org) {
				if (org.heap_ != nullptr) {
					heap_ = new T[num_];
					(void)memcpy_s(heap_, num_, org.heap_, org.num_);
				}
			}
			return *this;
		}

		//添え字演算子オーバーロード
		T operator[](std::int32_t index) const
		{
			return *(heap_ + index);
		}
		T& operator[](std::int32_t index)
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
	};


	//----------------------------------------------------------
	//
	// バイナリ操作メソッド群
	//
	//----------------------------------------------------------

	class BinWriter {
	public:
		//1バイトを書き込み(LE)
		static void write1ByteLe(std::uint8_t* const data, const std::uint8_t writeData);
		//2バイトを書き込み(LE)
		static void write2ByteLe(std::uint8_t* const data, const std::uint16_t writeData);
		//4バイトを書き込み(LE)
		static void write4ByteLe(std::uint8_t* const data, const std::uint32_t writeData);
	};

	class BinReader {
	public:
		//1バイトを読み込み(LE)
		static void read1ByteLe(const std::uint8_t* const data, std::uint8_t* const readData);
		//2バイトを読み込み(LE)
		static void read2ByteLe(const std::uint8_t* const data, std::uint16_t* const readData);
		//4バイトを読み込み(LE)
		static void read4ByteLe(const std::uint8_t* const data, std::uint32_t* const readData);
	};


	//----------------------------------------------------------
	//
	// 共通構造体定義
	//
	//----------------------------------------------------------

	struct CoordI16 {
		std::int16_t	x;
		std::int16_t	y;
		std::int16_t	z;
	};

	struct CoordI32 {
		std::int32_t	x;
		std::int32_t	y;
		std::int32_t	z;
	};

	struct CoordF64 {
		std::float64_t	x;
		std::float64_t	y;
		std::float64_t	z;
	};

	struct Color {
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
		std::uint8_t	a;
	};

	struct Size {
		std::int16_t	w;
		std::int16_t	h;
	};

	struct MapArea {
		std::int32_t	xmin;
		std::int32_t	ymin;
		std::int32_t	xmax;
		std::int32_t	ymax;
	};

	using AColor = HeapMemory<Color>;

	using ACoordI32 = HeapMemory<CoordI32>;

	using ABinary = HeapMemory<std::uint8_t>;

	using AImage = HeapMemory<std::uint8_t>;
}

#endif //INCLUDED_FWSTD_HPP
