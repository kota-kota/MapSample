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

    //タッチイベント
    enum TouchEvent {
        TOUCH_ON,
        TOUCH_OFF,
        TOUCH_MOVE,
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
        //座標設定
        inline void set(const T x, const T y);
        //X座標取得・設定・移動
        T getX() const;
        void setX(const T x);
        void moveX(const T xofs);
        //Y座標取得・設定・移動
        T getY() const;
        void setY(const T y);
        void moveY(const T yofs);
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
        //座標設定
        inline void set(const T x, const T y, const T z);
        //X座標取得・設定・移動
        inline T getX() const;
        inline void setX(const T x);
        inline void moveX(const T xofs);
        //Y座標取得・設定・移動
        inline T getY() const;
        inline void setY(const T y);
        inline void moveY(const T yofs);
        //Z座標取得・設定・移動
        inline T getZ() const;
        inline void setZ(const T z);
        inline void moveZ(const T zofs);
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
        //サイズ設定
        inline void set(const T width, const T height);
        //幅取得・設定
        inline T getWidth() const;
        inline void setWidth(const T width);
        //高さ取得・設定
        inline T getHeight() const;
        inline void setHeight(const T height);
    };

    //エリア
    template <typename T>
    class Area {
        //メンバ変数
        T xmin_;
        T ymin_;
        T xmax_;
        T ymax_;

    public:
        //コンストラクタ
        Area();
        Area(const T xmin, const T ymin, const T xmax, const T ymax);
        //エリア設定
        inline void set(const T xmin, const T ymin, const T xmax, const T ymax);
        //xmin取得・設定
        inline T getXMin() const;
        inline void setXMin(const T xmin);
        //ymin取得・設定
        inline T getYMin() const;
        inline void setYMin(const T ymin);
        //xmax取得・設定
        inline T getXMax() const;
        inline void setXMax(const T xmax);
        //ymax取得・設定
        inline T getYMax() const;
        inline void setYMax(const T ymax);
    };
}

#endif //INCLUDED_TYPE_HPP
