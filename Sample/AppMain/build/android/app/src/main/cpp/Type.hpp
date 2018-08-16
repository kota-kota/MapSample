#ifndef INCLUDED_TYPE_HPP
#define INCLUDED_TYPE_HPP

#include <cstdint>

namespace app {
    //型定義
    typedef char Char8;
    typedef char16_t Char16;
    typedef char32_t Char32;
    typedef std::int8_t Int8;
    typedef std::uint8_t UInt8;
    typedef std::int16_t Int16;
    typedef std::uint16_t UInt16;
    typedef std::int32_t Int32;
    typedef std::uint32_t UInt32;
    typedef std::int64_t Int64;
    typedef std::uint64_t UInt64;
    typedef float Float;
    typedef double Double;

    //リターンコード
    enum ReturnCode : Int8 {
        NG_ERROR = -1,
        OK = 0,
    };

    //位置(2D)
    template <typename T>
    class Pos2D {
        //メンバ変数
        T x_;
        T y_;

    public:
        //コンストラクタ
        Pos2D();
        Pos2D(const T x, const T y);
        //X座標取得・設定
        inline T getX() const;
        inline void setX(const T x);
        //Y座標取得・設定
        inline T getY() const;
        inline void setY(const T y);
    };

    //位置(3D)
    template <typename T>
    class Pos3D {
        //メンバ変数
        T x_;
        T y_;
        T z_;

    public:
        //コンストラクタ
        Pos3D();
        Pos3D(const T x, const T y, const T z);
        //X座標取得・設定
        inline T getX() const;
        inline void setX(const T x);
        //Y座標取得・設定
        inline T getY() const;
        inline void setY(const T y);
        //Z座標取得・設定
        inline T getZ() const;
        inline void setZ(const T z);
    };

    //サイズ
    template <typename T>
    class Size {
        //メンバ変数
        T width_;
        T height_;

    public:
        //コンストラクタ
        Size();
        Size(const T width, const T height);
        //幅取得・設定
        inline T getWidth() const;
        inline void setWidth(const T width);
        //高さ取得・設定
        inline T getHeight() const;
        inline void setHeight(const T height);
    };

    //エリア
    struct Area {
        Int32 xmin;
        Int32 ymin;
        Int32 xmax;
        Int32 ymax;
    };
}

#endif //INCLUDED_TYPE_HPP
