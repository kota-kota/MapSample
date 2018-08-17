#include "Type.hpp"

namespace app {

    //------------------------------------------------------------------------------------
    // Pos2D

    //コンストラクタ
    template <typename T>
    Pos2D<T>::Pos2D() : x_(0), y_(0) {}
    template <typename T>
    Pos2D<T>::Pos2D(const T x, const T y) : x_(x), y_(y) {}

    //座標設定
    template <typename T>
    void Pos2D<T>::set(const T x, const T y)
    {
        this->x_ = x;
        this->y_ = y;
    }

    //X座標取得・設定・移動
    template <typename T>
    T Pos2D<T>::getX() const { return this->x_; }
    template <typename T>
    void Pos2D<T>::setX(const T x) { this->x_ = x; }
    template <typename T>
    void Pos2D<T>::moveX(const T xofs) { this->x_ += xofs; }

    //Y座標取得・設定・移動
    template <typename T>
    T Pos2D<T>::getY() const { return this->y_; }
    template <typename T>
    void Pos2D<T>::setY(const T y) { this->y_ = y; }
    template <typename T>
    void Pos2D<T>::moveY(const T yofs) { this->y_ += yofs; }



    //------------------------------------------------------------------------------------
    // Pos3D

    //コンストラクタ
    template <typename T>
    Pos3D<T>::Pos3D() : x_(0), y_(0) {}
    template <typename T>
    Pos3D<T>::Pos3D(const T x, const T y, const T z) : x_(x), y_(y), z_(z) {}

    //座標設定
    template <typename T>
    void Pos3D<T>::set(const T x, const T y, const T z)
    {
        this->x_ = x;
        this->y_ = y;
        this->z_ = z;
    }

    //X座標取得・設定・移動
    template <typename T>
    T Pos3D<T>::getX() const { return this->x_; }
    template <typename T>
    void Pos3D<T>::setX(const T x) { this->x_ = x; }
    template <typename T>
    void Pos3D<T>::moveX(const T xofs) { this->x_ += xofs; }

    //Y座標取得・設定・移動
    template <typename T>
    T Pos3D<T>::getY() const { return this->y_; }
    template <typename T>
    void Pos3D<T>::setY(const T y) { this->y_ = y; }
    template <typename T>
    void Pos3D<T>::moveY(const T yofs) { this->y_ += yofs; }

    //Z座標取得・設定・移動
    template <typename T>
    T Pos3D<T>::getZ() const { return this->z_; }
    template <typename T>
    void Pos3D<T>::setZ(const T z) { this->z_ = z; }
    template <typename T>
    void Pos3D<T>::moveZ(const T zofs) { this->z_ += zofs; }


    //------------------------------------------------------------------------------------
    // Size

    //コンストラクタ
    template <typename T>
    Size<T>::Size() : width_(0), height_(0) {}
    template <typename T>
    Size<T>::Size(const T width, const T height) : width_(width), height_(height) {}

    //サイズ設定
    template <typename T>
    void Size<T>::set(const T width, const T height)
    {
        this->width_ = width;
        this->height_ = height;
    }

    //幅取得・設定
    template <typename T>
    T Size<T>::getWidth() const { return this->width_; }
    template <typename T>
    void Size<T>::setWidth(const T width) { this->width_ = width; }

    //高さ取得・設定
    template <typename T>
    T Size<T>::getHeight() const { return this->height_; }
    template <typename T>
    void Size<T>::setHeight(const T height) { this->height_ = height; }



    //------------------------------------------------------------------------------------
    // Area

    //コンストラクタ
    template <typename T>
    Area<T>::Area() : xmin_(0), ymin_(0), xmax_(0), ymax_(0) {}
    template <typename T>
    Area<T>::Area(const T xmin, const T ymin, const T xmax, const T ymax) : xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax) {}

    //エリア設定
    template <typename T>
    void Area<T>::set(const T xmin, const T ymin, const T xmax, const T ymax)
    {
        this->xmin_ = xmin;
        this->ymin_ = ymin;
        this->xmax_ = xmax;
        this->ymax_ = ymax;
    }

    //xmin取得・設定
    template <typename T>
    T Area<T>::getXMin() const { return this->xmin_; }
    template <typename T>
    void Area<T>::setXMin(const T xmin) { this->xmin_ = xmin; }

    //ymin取得・設定
    template <typename T>
    T Area<T>::getYMin() const { return this->ymin_; }
    template <typename T>
    void Area<T>::setYMin(const T ymin) { this->ymin_ = ymin; }

    //xmax取得・設定
    template <typename T>
    T Area<T>::getXMax() const { return this->xmax_; }
    template <typename T>
    void Area<T>::setXMax(const T xmax) { this->xmax_ = xmax; }

    //ymax取得・設定
    template <typename T>
    T Area<T>::getYMax() const { return this->ymax_; }
    template <typename T>
    void Area<T>::setYMax(const T ymax) { this->ymax_ = ymax; }



    //------------------------------------------------------------------------------------
    // 明示的なインスタンス化

    template class Pos2D<Int32>;
    template class Pos2D<Float>;
    template class Pos3D<Int32>;
    template class Pos3D<Float>;
    template class Size<Int32>;
    template class Area<Int32>;
    template class Area<Float>;
}
