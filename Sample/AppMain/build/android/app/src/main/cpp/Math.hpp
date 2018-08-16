#ifndef INCLUDED_MATH_HPP
#define INCLUDED_MATH_HPP

#include "Type.hpp"

namespace app {

    //Mathクラスのテスト関数
    Int32 test_Math();

    //Math共通
    class Math {
        //サインテーブル
        static const Int32 sinDivide_;
        static Double* sinTable_;

    public:
        //PI
        static const Double PI_D;
        static const Float PI_F;

    public:
        //サイン
        static Float sin(const Float deg);
        //コサイン
        static Float cos(const Float deg);

    private:
        //サインテーブル作成
        static void createSinTable();
        //サインテーブル検索
        static Double findSinTable(const Int32 idx);
    };

    //角度
    class Angle {
    public:
        enum Type {
            DEGREE,
            RADIAN,
        };

    private:
        //メンバ変数
        Float angle_;
        Type type_;

    public:
        //コンストラクタ
        Angle();
        Angle(const Float angle, const Type type);
        //度単位で角度取得
        Float degree() const;
        //ラジアン単位で角度取得
        Float radian() const;
    };

    //4*4行列
    class Matrix4F {
        //メンバ変数
        static const Int32 ROW = 4;
        static const Int32 COL = 4;
        Float mat_[ROW][COL];

    public:
        //コンストラクタ
        Matrix4F();
        Matrix4F(Float x00, Float x01, Float x02, Float x03,
                 Float x10, Float x11, Float x12, Float x13,
                 Float x20, Float x21, Float x22, Float x23,
                 Float x30, Float x31, Float x32, Float x33);
        //デストラクタ
        ~Matrix4F();
        //コピーコンストラクタ
        Matrix4F(const Matrix4F& org);
        //代入演算子 =Matrix4F(A)
        Matrix4F& operator=(const Matrix4F& A);
        //代入演算子 +=Matrix4F(A)
        Matrix4F& operator+=(const Matrix4F& A);
        //代入演算子 -=Matrix4F(A)
        Matrix4F& operator-=(const Matrix4F& A);
        //代入演算子 *=float(k)
        Matrix4F& operator*=(Float k);
        //代入演算子 *=Matrix4F(A)
        Matrix4F& operator*=(const Matrix4F& A);
        //代入演算子 /=float(k)
        Matrix4F& operator/=(Float k);
        //算術演算子 Matrix4F(this)+Matrix4F(A)
        Matrix4F operator+(const Matrix4F& A);
        //算術演算子 Matrix4F(this)-Matrix4F(A)
        Matrix4F operator-(const Matrix4F& A);
        //算術演算子 Matrix4F(this)*float(k)
        Matrix4F operator*(const Float& k);
        //算術演算子 Matrix4F(this)*Matrix4F(A)
        Matrix4F operator*(const Matrix4F& A);
        //算術演算子 Matrix4F(this)/float(k)
        Matrix4F operator/(const Float& k);
        //添え字演算子 Matrix4F[i]
        Float* operator[](Int32 i);
        const Float* operator[](Int32 i) const;
        //単位行列
        void identify();
        //平行移動行列
        void translate(const Float x, const Float y, const Float z);
        //回転行列
        void rotate(const Angle x, const Angle y, const Angle z);
        //拡大縮小行列
        void scale(const Float x, const Float y, const Float z);
        //正射影行列 垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
        void orthogonal(const Float left_x, const Float right_x, const Float bottom_y, const Float top_y, const Float near_z, const Float far_z);
        //透視射影行列 垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
        void frustum(const Float left_x, const Float right_x, const Float bottom_y, const Float top_y, const Float near_z, const Float far_z);
        //初期化
        void init();
        //転置
        void transpose();
    };
}

#endif //INCLUDED_MATH_HPP
