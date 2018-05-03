#include "DrawIF.hpp"
#define _USE_MATH_DEFINES	//M_PI用
#include <math.h>

namespace fw {

	//ラジアンから角度に変換
	std::float32_t DrawIF::convRadian2Degree(const std::float32_t rad)
	{
		return rad * 180.0F / static_cast<std::float32_t>(M_PI);
	}

	//角度からラジアンに変換
	std::float32_t DrawIF::convDegree2Radian(const std::float32_t deg)
	{
		return deg * static_cast<std::float32_t>(M_PI) / 180.0F;
	}

	//単位行列を取得
	MatrixF DrawIF::identifyMatrixF()
	{
		MatrixF matrix = { 0.0F };
		matrix.mat[0] = 1.0F;
		matrix.mat[5] = 1.0F;
		matrix.mat[10] = 1.0F;
		matrix.mat[15] = 1.0F;
		return matrix;
	}

	//行列の積
	MatrixF DrawIF::multiplyMatrix(const MatrixF& m1, const MatrixF& m2)
	{
		MatrixF matrix = { 0.0 };
		for (int32_t row = 0; row < 4; row++) {			//行
			for (int32_t col = 0; col < 4; col++) {		//列
				std::int32_t i = (row * 4) + col;
				matrix.mat[i] =
					(m1.mat[(row * 4)] * m2.mat[col]) +
					(m1.mat[(row * 4) + 1] * m2.mat[col + 4]) +
					(m1.mat[(row * 4) + 2] * m2.mat[col + 8]) +
					(m1.mat[(row * 4) + 3] * m2.mat[col + 12]);
			}
		}
		return matrix;
	}

	//行列の転置
	MatrixF DrawIF::transposeMatrix(const MatrixF& m)
	{
		MatrixF matrix = { 0.0F };

		matrix.mat[0] = m.mat[0];
		matrix.mat[1] = m.mat[4];
		matrix.mat[2] = m.mat[8];
		matrix.mat[3] = m.mat[12];

		matrix.mat[4] = m.mat[1];
		matrix.mat[5] = m.mat[5];
		matrix.mat[6] = m.mat[9];
		matrix.mat[7] = m.mat[13];

		matrix.mat[8] = m.mat[2];
		matrix.mat[9] = m.mat[6];
		matrix.mat[10] = m.mat[10];
		matrix.mat[11] = m.mat[14];

		matrix.mat[12] = m.mat[3];
		matrix.mat[13] = m.mat[7];
		matrix.mat[14] = m.mat[11];
		matrix.mat[15] = m.mat[15];

		return matrix;
	}

	//平行移動行列の生成
	MatrixF DrawIF::translateMatrix(const VectorF& trans)
	{
		MatrixF matrix = { 0.0 };

		//単位行列の生成
		matrix = identifyMatrixF();

		//平行移動行列の生成
		matrix.mat[3] = trans.x;
		matrix.mat[7] = trans.y;
		matrix.mat[11] = trans.z;
		return matrix;
	}

	//回転行列の生成
	MatrixF DrawIF::rotateMatrix(const VectorF& rotate)
	{
		//入力された回転角度を度からラジアンに変換
		VectorF rad = { 0.0 };
		rad.x = convDegree2Radian(rotate.x);
		rad.y = convDegree2Radian(rotate.y);
		rad.z = convDegree2Radian(rotate.z);

		//x方向の回転行列
		MatrixF matX = { 0.0 };
		matX = identifyMatrixF();
		if (rad.x != 0.0) {
			//コサイン,サインを計算
			std::float32_t c = cosf(rad.x);
			std::float32_t s = sinf(rad.x);

			matX.mat[5] = c;
			matX.mat[6] = -s;
			matX.mat[9] = s;
			matX.mat[10] = c;
		}

		//y方向の回転行列
		MatrixF matY = { 0.0 };
		matY = identifyMatrixF();
		if (rad.y != 0.0) {
			//コサイン,サインを計算
			std::float32_t c = cosf(rad.y);
			std::float32_t s = sinf(rad.y);

			matY.mat[0] = c;
			matY.mat[2] = s;
			matY.mat[8] = -s;
			matY.mat[10] = c;
		}

		//z方向の回転行列
		MatrixF matZ = { 0.0 };
		matZ = identifyMatrixF();
		if (rad.z != 0.0) {
			//コサイン,サインを計算
			std::float32_t c = cosf(rad.z);
			std::float32_t s = sinf(rad.z);

			matZ.mat[0] = c;
			matZ.mat[1] = -s;
			matZ.mat[4] = s;
			matZ.mat[5] = c;
		}

		//XYZ方向の行列の積から回転行列を生成
		MatrixF matrix = { 0.0 };
		matrix = multiplyMatrix(matX, matY);
		matrix = multiplyMatrix(matrix, matZ);
		return matrix;
	}

	//拡大縮小行列の生成
	MatrixF DrawIF::scaleMatrix(const VectorF& scale)
	{
		MatrixF matrix = { 0.0 };

		//単位行列の生成
		matrix = identifyMatrixF();

		//拡大縮小行列の生成
		matrix.mat[0] = scale.x;
		matrix.mat[5] = scale.y;
		matrix.mat[10] = scale.z;
		return matrix;
	}

	//モデル変換行列の生成
	MatrixF DrawIF::modelMatrix(const VectorF& trans, const VectorF& rotate, const VectorF& scale)
	{
		//拡大縮小変換行列を生成
		MatrixF matScale = { 0.0 };
		matScale = scaleMatrix(scale);

		//回転変換行列を生成
		MatrixF matRotate = { 0.0 };
		matRotate = rotateMatrix(rotate);

		// 平行移動変換行列を生成
		MatrixF matTranslate = { 0.0 };
		matTranslate = translateMatrix(trans);

		// モデル変換行列を生成
		MatrixF matrix = { 0.0 };
		matrix = multiplyMatrix(matScale, matRotate);
		matrix = multiplyMatrix(matrix, matTranslate);
		return matrix;
	}

	//プロジェクション行列生成(orthogonal)
	//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
	MatrixF DrawIF::makeProjMatrix_Ortho(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar)
	{
		MatrixF matrix = { 0.0F };

		//1行目
		matrix.mat[0] = 2.0F / (right - left);
		matrix.mat[1] = 0.0F;
		matrix.mat[2] = 0.0F;
		matrix.mat[3] = -(right + left) / (right - left);

		//2行目
		matrix.mat[4] = 0.0F;
		matrix.mat[5] = 2.0F / (top - bottom);
		matrix.mat[6] = 0.0F;
		matrix.mat[7] = -(top + bottom) / (top - bottom);

		//3行目
		matrix.mat[8] = 0.0F;
		matrix.mat[9] = 0.0F;
		matrix.mat[10] = -2.0F / (zfar - znear);
		matrix.mat[11] = -(zfar + znear) / (zfar - znear);

		//4行目
		matrix.mat[12] = 0.0F;
		matrix.mat[13] = 0.0F;
		matrix.mat[14] = 0.0F;
		matrix.mat[15] = 1.0F;

		return matrix;
	}

	//プロジェクション行列生成(frustum)
	//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
	MatrixF DrawIF::makeProjMatrix_Frustum(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar)
	{
		MatrixF matrix = { 0.0F };

		//1行目
		matrix.mat[0] = (2.0F * znear) / (right - left);
		matrix.mat[1] = 0.0F;
		matrix.mat[2] = (right + left) / (right - left);
		matrix.mat[3] = 0.0F;

		//2行目
		matrix.mat[4] = 0.0F;
		matrix.mat[5] = (2.0F * znear) / (top - bottom);
		matrix.mat[6] = (top + bottom) / (top - bottom);
		matrix.mat[7] = 0.0F;

		//3行目
		matrix.mat[8] = 0.0F;
		matrix.mat[9] = 0.0F;
		matrix.mat[10] = -(zfar + znear) / (zfar - znear);
		matrix.mat[11] = -(2.0F * zfar * znear) / (zfar - znear);

		//4行目
		matrix.mat[12] = 0.0F;
		matrix.mat[13] = 0.0F;
		matrix.mat[14] = -1.0F;
		matrix.mat[15] = 0.0F;

		return matrix;
	}
}
