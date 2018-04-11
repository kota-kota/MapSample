#include "DrawIF.hpp"

namespace draw {

	//行列の転置
	MatrixF DrawIF::transMatrix(const MatrixF& m)
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
