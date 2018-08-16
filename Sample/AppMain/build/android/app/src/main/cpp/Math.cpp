#include "Math.hpp"
#include <cmath>


namespace app {

    //Mathクラスのテスト関数
    Int32 test_Math()
    {
        Float s1 = Math::sin(0.875f);
        Float s2 = Math::sin(800);
        Float s3 = Math::sin(-800);
        Float s4 = Math::sin(-200.6798f);
        Float c1 = Math::cos(0.875f);
        Float c2 = Math::cos(800);
        Float c3 = Math::cos(-800);
        Float c4 = Math::cos(-200.6798f);

        Matrix4F m1;
        m1.identify();
        m1.rotate(Angle(10.0F, Angle::Type::DEGREE), Angle(20.0F, Angle::Type::DEGREE), Angle(30.0F, Angle::Type::DEGREE));

        return 0;
    }


    //------------------------------------------------------------------------------------
    // Mathクラス

    //サインテーブル
    const Int32 Math::sinDivide_ = 16;  //1度を16分割
    Double* Math::sinTable_ = nullptr;

    //PI
    const Double Math::PI_D = 3.14159265358979323846;
    const Float Math::PI_F = static_cast<Float>(Math::PI_D);

    //サイン
    Float Math::sin(const Float deg)
    {
        createSinTable();

        //sin(deg)
        const Int32 idx = static_cast<Int32>(deg * static_cast<Float>(sinDivide_));
        return static_cast<Float>(findSinTable(idx));
    }

    //コサイン
    Float Math::cos(const Float deg)
    {
        createSinTable();

        //cos(deg) -> sin(deg + 90)
        const Int32 idx = static_cast<Int32>(deg * static_cast<Float>(sinDivide_));
        return static_cast<Float>(findSinTable(idx + 90 * sinDivide_));
    }

    //サインテーブル作成
    void Math::createSinTable()
    {
        if(sinTable_ == nullptr) {
            sinTable_ = new Double[90 * sinDivide_ + 1];
            for(Int32 i = 0; i <= 90 * sinDivide_; i++) {
                const Float deg = static_cast<Float>(i) / static_cast<Float>(sinDivide_);
                const Angle angle(deg, Angle::Type::DEGREE);
                sinTable_[i] = ::sin(angle.radian());
            }
        }
    }

    //サインテーブル検索
    Double Math::findSinTable(const Int32 idx)
    {
        Int32 idx_ = idx;
        while(idx_ < 0) {
            idx_ += 360 * sinDivide_;
        }
        while(idx_ > (360 * sinDivide_)) {
            idx_ -= 360 * sinDivide_;
        }

        Double ret;
        if(idx_ <= 90 * sinDivide_) {
            //deg:0 ~ 90 -> sin(deg)
            ret = sinTable_[idx_];
        }
        else if(idx_ <= 180 * sinDivide_) {
            //deg:90 ~ 180 -> sin(180 - deg)
            ret = sinTable_[180 * sinDivide_ - idx_];
        }
        else if(idx_ <= 270 * sinDivide_) {
            //deg:180 ~ 270 -> -sin(deg - 180)
            ret = -sinTable_[idx_ - 180 * sinDivide_];
        }
        else {
            //deg:270 ~ 360 -> -sin(360 - deg)
            ret = -sinTable_[360 * sinDivide_ - idx_];
        }

        return ret;
    }



    //------------------------------------------------------------------------------------
    // Angleクラス

    //コンストラクタ
    Angle::Angle() :
            angle_(0), type_(DEGREE)
    {

    }
    Angle::Angle(const Float angle, const Type type) :
            angle_(angle), type_(type)
    {
    }

    //度単位で角度取得
    Float Angle::degree() const
    {
        Float ret = this->angle_;
        if(this->type_ == RADIAN) {
            ret = this->angle_ * 180.0F / Math::PI_F;
        }
        return ret;
    }

    //ラジアン単位で角度取得
    Float Angle::radian() const
    {
        Float ret = this->angle_;
        if(this->type_ == DEGREE) {
            ret = this->angle_ * Math::PI_F / 180.0F;
        }
        return ret;
    }



    //------------------------------------------------------------------------------------
    // Matrix4Fクラス

    //コンストラクタ
    Matrix4F::Matrix4F()
    {
        for(Int32 row = 0; row < ROW; row++){
            for(Int32 col = 0; col < COL; col++) {
                mat_[row][col] = 0.0F;
            }
        }
    }
    Matrix4F::Matrix4F(
            Float x00, Float x01, Float x02, Float x03,
            Float x10, Float x11, Float x12, Float x13,
            Float x20, Float x21, Float x22, Float x23,
            Float x30, Float x31, Float x32, Float x33)
    {
        mat_[0][0]=x00;mat_[0][1]=x01;mat_[0][2]=x02;mat_[0][2]=x03;
        mat_[1][0]=x10;mat_[1][1]=x11;mat_[1][2]=x12;mat_[1][2]=x13;
        mat_[2][0]=x20;mat_[2][1]=x21;mat_[2][2]=x22;mat_[2][2]=x23;
        mat_[3][0]=x30;mat_[3][1]=x31;mat_[3][2]=x32;mat_[3][3]=x33;
    }

    //デストラクタ
    Matrix4F::~Matrix4F()
    {
    }

    //コピーコンストラクタ
    Matrix4F::Matrix4F(const Matrix4F& org)
    {
        *this = org;
    }

    //代入演算子 =Matrix4F(A)
    Matrix4F& Matrix4F::operator=(const Matrix4F& A)
    {
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] = A.mat_[row][col];
            }
        }
        return *this;
    }
    //代入演算子 +=Matrix4F(A)
    Matrix4F& Matrix4F::operator+=(const Matrix4F& A)
    {
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] += A.mat_[row][col];
            }
        }
        return *this;
    }
    //代入演算子 -=Matrix4F(A)
    Matrix4F& Matrix4F::operator-=(const Matrix4F& A)
    {
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] -= A.mat_[row][col];
            }
        }
        return *this;
    }
    //代入演算子 *=float(k)
    Matrix4F& Matrix4F::operator*=(Float k)
    {
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] *= k;
            }
        }
        return *this;
    }
    //代入演算子 *=Matrix4F(A)
    Matrix4F& Matrix4F::operator*=(const Matrix4F& A)
    {
        Matrix4F ret;
        for (Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                for (Int32 i = 0; i < ROW; i++)
                    ret.mat_[row][col] += this->mat_[row][i] * A.mat_[i][col];
            }
        }
        *this = ret;
        return *this;
    }
    //代入演算子 /=float(k)
    Matrix4F& Matrix4F::operator/=(Float k)
    {
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] /= k;
            }
        }
        return *this;
    }

    //算術演算子 Matrix4F(this)+Matrix4F(A)
    Matrix4F Matrix4F::operator+(const Matrix4F& A)
    {
        Matrix4F ret;
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                ret.mat_[row][col] = this->mat_[row][col] + A.mat_[row][col];
            }
        }
        return ret;
    }

    //算術演算子 Matrix4F(this)-Matrix4F(A)
    Matrix4F Matrix4F::operator-(const Matrix4F& A)
    {
        Matrix4F ret;
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                ret.mat_[row][col] = this->mat_[row][col] - A.mat_[row][col];
            }
        }
        return ret;
    }

    //算術演算子 Matrix4F(this)*float(k)
    Matrix4F Matrix4F::operator*(const Float& k)
    {
        Matrix4F ret;
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                ret.mat_[row][col] = this->mat_[row][col] * k;
            }
        }
        return ret;
    }

    //算術演算子 Matrix4F(this)*Matrix4F(A)
    Matrix4F Matrix4F::operator*(const Matrix4F& A)
    {
        Matrix4F ret;
        for (app::Int32 row = 0; row < ROW; row++) {
            for (app::Int32 col = 0; col < COL; col++) {
                for (app::Int32 i = 0; i < ROW; i++) {
                    ret.mat_[row][col] += this->mat_[row][i] * A.mat_[i][col];
                }
            }
        }
        return ret;
    }

    //算術演算子 Matrix4F(this)/float(k)
    Matrix4F Matrix4F::operator/(const Float& k)
    {
        Matrix4F ret;
        for(Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                ret.mat_[row][col] = this->mat_[row][col] / k;
            }
        }
        return ret;
    }

    //添え字演算子
    Float* Matrix4F::operator[](Int32 i)
    {
        return this->mat_[i];
    }
    const Float* Matrix4F::operator[](Int32 i) const
    {
        return this->mat_[i];
    }

    //単位行列
    void Matrix4F::identify()
    {
        for(Int32 row = 0; row < ROW; row++){
            for(Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] = (row == col) ? 1.0F : 0.0F;
            }
        }
    }

    //平行移動行列
    void Matrix4F::translate(const Float x, const Float y, const Float z)
    {
        this->identify();
        this->mat_[0][3] = x;
        this->mat_[1][3] = y;
        this->mat_[2][3] = z;
    }

    //回転行列
    void Matrix4F::rotate(const Angle x, const Angle y, const Angle z)
    {
        //ラジアン角度を取得
        Float rad_x = x.radian();
        Float rad_y = y.radian();
        Float rad_z = z.radian();

        //X方向の回転行列
        Matrix4F matX;
        matX.identify();
        if (rad_x != 0.0F) {
            //コサイン,サインを計算
            const Float c = Math::cos(rad_x);
            const Float s = Math::sin(rad_x);

            matX.mat_[1][1] = c;
            matX.mat_[1][2] = -s;
            matX.mat_[2][1] = s;
            matX.mat_[2][2] = c;
        }

        //Y方向の回転行列
        Matrix4F matY;
        matY.identify();
        if (rad_y != 0.0) {
            //コサイン,サインを計算
            const Float c = Math::cos(rad_y);
            const Float s = Math::sin(rad_y);

            matY.mat_[0][0] = c;
            matY.mat_[0][2] = s;
            matY.mat_[2][0] = -s;
            matY.mat_[2][2] = c;
        }

        //z方向の回転行列
        Matrix4F matZ;
        matZ.identify();
        if (rad_z != 0.0) {
            //コサイン,サインを計算
            const Float c = Math::cos(rad_z);
            const Float s = Math::sin(rad_z);

            matZ.mat_[0][0] = c;
            matZ.mat_[0][1] = -s;
            matZ.mat_[1][0] = s;
            matZ.mat_[1][1] = c;
        }

        //XYZ方向の行列の積から回転行列を生成
        *this = matX * matY * matZ;
    }

    //拡大縮小行列
    void Matrix4F::scale(const Float x, const Float y, const Float z)
    {
        this->identify();
        this->mat_[0][0] = x;
        this->mat_[1][1] = y;
        this->mat_[2][2] = z;
    }

    //正射影行列 垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
    void Matrix4F::orthogonal(const Float left_x, const Float right_x, const Float bottom_y, const Float top_y, const Float near_z, const Float far_z)
    {
        //1行目
        this->mat_[0][0] = 2.0F / (right_x - left_x);
        this->mat_[0][1] = 0.0F;
        this->mat_[0][2] = 0.0F;
        this->mat_[0][3] = -(right_x + left_x) / (right_x - left_x);
        //2行目
        this->mat_[1][0] = 0.0F;
        this->mat_[1][1] = 2.0F / (top_y - bottom_y);
        this->mat_[1][2] = 0.0F;
        this->mat_[1][3] = -(top_y + bottom_y) / (top_y - bottom_y);
        //3行目
        this->mat_[2][0] = 0.0F;
        this->mat_[2][1] = 0.0F;
        this->mat_[2][2] = -2.0F / (far_z - near_z);
        this->mat_[2][3] = -(far_z + near_z) / (far_z - near_z);
        //4行目
        this->mat_[3][0] = 0.0F;
        this->mat_[3][1] = 0.0F;
        this->mat_[3][2] = 0.0F;
        this->mat_[2][3] = 1.0F;
    }

    //透視射影行列 垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
    void Matrix4F::frustum(const Float left_x, const Float right_x, const Float bottom_y, const Float top_y, const Float near_z, const Float far_z)
    {
        //1行目
        this->mat_[0][0] = (2.0F * near_z) / (right_x - left_x);
        this->mat_[0][1] = 0.0F;
        this->mat_[0][2] = (right_x + left_x) / (right_x - left_x);
        this->mat_[0][3] = 0.0F;
        //2行目
        this->mat_[1][0] = 0.0F;
        this->mat_[1][1] = (2.0F * near_z) / (top_y - bottom_y);
        this->mat_[1][2] = (top_y + bottom_y) / (top_y - bottom_y);
        this->mat_[1][3] = 0.0F;
        //3行目
        this->mat_[2][0] = 0.0F;
        this->mat_[2][1] = 0.0F;
        this->mat_[2][2] = -(far_z + near_z) / (far_z - near_z);
        this->mat_[2][3] = -(2.0F * far_z * near_z) / (far_z - near_z);
        //4行目
        this->mat_[3][0] = 0.0F;
        this->mat_[3][1] = 0.0F;
        this->mat_[3][2] = -1.0F;
        this->mat_[3][3] = 0.0F;
    }

    //初期化
    void Matrix4F::init()
    {
        for(Int32 row = 0; row < ROW; row++){
            for(Int32 col = 0; col < COL; col++) {
                this->mat_[row][col] = 0.0F;
            }
        }
    }

    //転置
    void Matrix4F::transpose() {
        Matrix4F trans;
        for (Int32 row = 0; row < ROW; row++) {
            for (Int32 col = 0; col < COL; col++) {
                trans.mat_[row][col] = this->mat_[col][row];
            }
        }
        *this = trans;
    }
}
