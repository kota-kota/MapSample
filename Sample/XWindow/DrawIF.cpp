#include "DrawIF.hpp"

namespace draw {

	//行列の転置
	MatrixF DrawIF::transMatrix(const MatrixF& m)
	{
		MatrixF matrix = { 0.0F };

		matrix.mat[0][0] = m.mat[0][0];
		matrix.mat[0][1] = m.mat[1][0];
		matrix.mat[0][2] = m.mat[2][0];
		matrix.mat[0][3] = m.mat[3][0];

		matrix.mat[1][0] = m.mat[0][1];
		matrix.mat[1][1] = m.mat[1][1];
		matrix.mat[1][2] = m.mat[2][1];
		matrix.mat[1][3] = m.mat[3][1];

		matrix.mat[2][0] = m.mat[0][2];
		matrix.mat[2][1] = m.mat[1][2];
		matrix.mat[2][2] = m.mat[2][2];
		matrix.mat[2][3] = m.mat[3][2];

		matrix.mat[3][0] = m.mat[0][3];
		matrix.mat[3][1] = m.mat[1][3];
		matrix.mat[3][2] = m.mat[2][3];
		matrix.mat[3][3] = m.mat[3][3];

		return matrix;
	}
	//プロジェクション行列生成(orthogonal)
	//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
	MatrixF DrawIF::makeProjMatrix_Ortho(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar)
	{
		MatrixF matrix = { 0.0F };

		//1行目
		matrix.mat[0][0] = 2.0F / (right - left);
		matrix.mat[0][1] = 0.0F;
		matrix.mat[0][2] = 0.0F;
		matrix.mat[0][3] = -(right + left) / (right - left);

		//2行目
		matrix.mat[1][0] = 0.0F;
		matrix.mat[1][1] = 2.0F / (top - bottom);
		matrix.mat[1][2] = 0.0F;
		matrix.mat[1][3] = -(top + bottom) / (top - bottom);

		//3行目
		matrix.mat[2][0] = 0.0F;
		matrix.mat[2][1] = 0.0F;
		matrix.mat[2][2] = -2.0F / (zfar - znear);
		matrix.mat[2][3] = -(zfar + znear) / (zfar - znear);

		//4行目
		matrix.mat[3][0] = 0.0F;
		matrix.mat[3][1] = 0.0F;
		matrix.mat[3][2] = 0.0F;
		matrix.mat[3][3] = 1.0F;

		return matrix;
	}

	//プロジェクション行列生成(frustum)
	//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
	MatrixF DrawIF::makeProjMatrix_Frustum(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar)
	{
		MatrixF matrix = { 0.0F };

		matrix.mat[0][0] = (2.0F * znear) / (right - left);
		matrix.mat[0][1] = 0.0F;
		matrix.mat[0][2] = (right + left) / (right - left);
		matrix.mat[0][3] = 0.0F;

		matrix.mat[1][0] = 0.0F;
		matrix.mat[1][1] = (2.0F * znear) / (top - bottom);
		matrix.mat[1][2] = (top + bottom) / (top - bottom);
		matrix.mat[1][3] = 0.0F;

		matrix.mat[2][0] = 0.0F;
		matrix.mat[2][1] = 0.0F;
		matrix.mat[2][2] = -(zfar + znear) / (zfar - znear);
		matrix.mat[2][3] = -(2.0F * zfar * znear) / (zfar - znear);

		matrix.mat[3][0] = 0.0F;
		matrix.mat[3][1] = 0.0F;
		matrix.mat[3][2] = -1.0F;
		matrix.mat[3][3] = 0.0F;

		return matrix;
	}
}
