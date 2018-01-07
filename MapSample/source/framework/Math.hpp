#ifndef INCLUDED_MATH_HPP
#define INCLUDED_MATH_HPP

#include "Std.hpp"

namespace fw {

	//3次元ベクトル
	template <typename T> struct Vector {
		T	x;
		T	y;
		T	z;
	};
	using VectorF = Vector<std::float_t>;
	using VectorD = Vector<std::double_t>;

	//4*4行列
	template <typename T> struct Matrix {
		T	mat[4][4];
	};
	using MatrixF = Matrix<std::float_t>;
	using MatrixD = Matrix<std::double_t>;

	class Math {
	public:
		//ラジアンから角度に変換
		static inline std::double_t convRadian2Degree(const std::double_t rad);
		//角度からラジアンに変換
		static inline std::double_t convDegree2Radian(const std::double_t deg);
		//VectorDからVectorFに変換
		static fw::VectorF convVectorD2VectorF(const fw::VectorD& vd);
		//MatrixDからMatrixFに変換
		static fw::MatrixF convMatrixD2MatrixF(const fw::MatrixD& md);

		//ベクトルの長さ
		static std::double_t lengthVector(const VectorD& v);
		//ベクトルの正規化
		static VectorD normalizeVector(const VectorD& v);
		//2つのベクトルの和
		static VectorD sumVector(const VectorD& v1, const VectorD& v2);
		//2つのベクトルの内積
		static std::double_t dotVector(const VectorD& v1, const VectorD& v2);
		//2つのベクトルの外積
		static VectorD crossVector(const VectorD& v1, const VectorD& v2);
		//2つのベクトルの成す角度
		static std::double_t degreeVector(const VectorD& v1, const VectorD& v2);

		//単位行列の生成
		static MatrixF identifyMatrixF();
		static MatrixD identifyMatrixD();
		//行列の積
		static MatrixD multiplyMatrix(const MatrixD& m1, const MatrixD& m2);
		//行列の転置
		static MatrixD transposeMatrix(const MatrixD& m);
		//平行移動行列の生成
		static MatrixD translateMatrix(const VectorD& trans);
		//回転行列の生成
		static MatrixD rotateMatrix(const VectorD& rotate);
		//拡大縮小行列の生成
		static MatrixD scaleMatrix(const VectorD& scale);
		//モデル変換行列の生成
		static MatrixD modelMatrix(const VectorD& trans, const VectorD& rotate, const VectorD& scale);
		//カメラ視野行列の生成(ortho)
		static MatrixD orthoMatrix(const std::double_t left, const std::double_t right, const std::double_t bottom, const std::double_t top, const std::double_t near, const std::double_t far);
		//カメラ視野行列の生成(frustum)
		static MatrixD frustumMatrix(const std::double_t left, const std::double_t right, const std::double_t bottom, const std::double_t top, const std::double_t near, const std::double_t far);
		//カメラ視野行列の生成(perspective)
		static MatrixD perspectiveMatrix(const std::double_t near, const std::double_t far, const std::double_t degFovy, const std::double_t aspect);
		//カメラ視点行列の生成(lookAt)
		static MatrixD lookMatrix(const VectorD& eye, const VectorD& look, const VectorD& up);
	};
}



#endif //INCLUDED_MATH_HPP
