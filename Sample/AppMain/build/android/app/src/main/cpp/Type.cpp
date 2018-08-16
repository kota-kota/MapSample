#include "Type.hpp"

namespace app {

    //------------------------------------------------------------------------------------
    // Pos2D

    //コンストラクタ
    template <typename T>
    Pos2D<T>::Pos2D() : x_(0), y_(0) {}
    template <typename T>
    Pos2D<T>::Pos2D(const T x, const T y) : x_(x), y_(y) {}

    //X座標取得・設定
    template <typename T>
    inline T Pos2D<T>::getX() const { return this->x_; }
    template <typename T>
    inline void Pos2D<T>::setX(const T x) { this->x_ = x; }

    //Y座標取得・設定
    template <typename T>
    inline T Pos2D<T>::getY() const { return this->y_; }
    template <typename T>
    inline void Pos2D<T>::setY(const T y) { this->y_ = y; }



    //------------------------------------------------------------------------------------
    // Pos3D

    //コンストラクタ
    template <typename T>
    Pos3D<T>::Pos3D() : x_(0), y_(0) {}
    template <typename T>
    Pos3D<T>::Pos3D(const T x, const T y, const T z) : x_(x), y_(y), z_(z) {}

    //X座標取得・設定
    template <typename T>
    inline T Pos3D<T>::getX() const { return this->x_; }
    template <typename T>
    inline void Pos3D<T>::setX(const T x) { this->x_ = x; }

    //Y座標取得・設定
    template <typename T>
    inline T Pos3D<T>::getY() const { return this->y_; }
    template <typename T>
    inline void Pos3D<T>::setY(const T y) { this->y_ = y; }

    //Z座標取得・設定
    template <typename T>
    inline T Pos3D<T>::getZ() const { return this->z_; }
    template <typename T>
    inline void Pos3D<T>::setZ(const T z) { this->z_ = z; }


    //------------------------------------------------------------------------------------
    // Size

    //コンストラクタ
    template <typename T>
    Size<T>::Size() : width_(0), height_(0) {}
    template <typename T>
    Size<T>::Size(const T width, const T height) : width_(width), height_(height) {}

    //幅取得・設定
    template <typename T>
    inline T Size<T>::getWidth() const { return this->width_; }
    template <typename T>
    inline void Size<T>::setWidth(const T width) { this->width_ = width; }

    //高さ取得・設定
    template <typename T>
    inline T Size<T>::getHeight() const { return this->height_; }
    template <typename T>
    inline void Size<T>::setHeight(const T height) { this->height_ = height; }



    //------------------------------------------------------------------------------------
    // 明示的なインスタンス化

    template class Pos2D<Int32>;
    template class Pos2D<Float>;
    template class Pos3D<Int32>;
    template class Pos3D<Float>;
    template class Size<Int32>;
}
