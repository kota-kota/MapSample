#include "Math.hpp"
#define _USE_MATH_DEFINES	//M_PI用
#include <math.h>


//ラジアンから角度に変換
std::double_t fw::Math::convRadian2Degree(const std::double_t rad)
{
	return rad * 180.0 / M_PI;
}

//角度からラジアンに変換
std::double_t fw::Math::convDegree2Radian(const std::double_t deg)
{
	return deg * M_PI / 180.0;
}

//VectorDからVectorFに変換
fw::VectorF fw::Math::convVectorD2VectorF(const fw::VectorD& vd)
{
	VectorF vf = { 0.0F };
	vf.x = std::float_t(vd.x);
	vf.y = std::float_t(vd.y);
	vf.z = std::float_t(vd.z);
	return vf;
}

//MatrixDからMatrixFに変換
fw::MatrixF fw::Math::convMatrixD2MatrixF(const fw::MatrixD& md)
{
	MatrixF mf = { 0.0F };
	for (int32_t row = 0; row < 4; row++) {			//行
		for (int32_t col = 0; col < 4; col++) {		//列
			mf.mat[row][col] = std::float_t(md.mat[row][col]);
		}
	}
	return mf;
}


//ベクトルの長さ
std::double_t fw::Math::lengthVector(const VectorD& v)
{
	//XYZ座標の2乗値を計算
	VectorD power = { (v.x * v.x), (v.y * v.y), (v.z * v.z) };

	//ベクトルの長さを計算
	std::double_t len = sqrt(power.x + power.y + power.z);
	return len;
}

//ベクトルの正規化
fw::VectorD fw::Math::normalizeVector(const VectorD& v)
{
	//ベクトルの長さ
	std::double_t len = lengthVector(v);

	//正規化
	VectorD vector = { (v.x / len), (v.y / len), (v.z / len) };
	return vector;
}

//2つのベクトルの和
fw::VectorD fw::Math::sumVector(const VectorD& v1, const VectorD& v2)
{
	VectorD vector = { (v1.x + v2.x), (v1.y + v2.y), (v1.z + v2.z) };
	return vector;
}

//2つのベクトルの内積
std::double_t fw::Math::dotVector(const VectorD& v1, const VectorD& v2)
{
	std::double_t dot = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return dot;
}

//2つのベクトルの外積
fw::VectorD fw::Math::crossVector(const VectorD& v1, const VectorD& v2)
{
	// 外積
	VectorD cross = { 0.0 };
	cross.x = (v1.y * v2.z) - (v1.z * v2.y);
	cross.y = (v1.z * v2.x) - (v1.x * v2.z);
	cross.z = (v1.x * v2.y) - (v1.y * v2.x);

	return cross;
}

//2つのベクトルの成す角度
std::double_t fw::Math::degreeVector(const VectorD& v1, const VectorD& v2)
{
	//v1とv2から成るベクトル
	VectorD dv = { (v2.x - v1.x), (v2.y - v1.y), 0.0F };
	if ((dv.x == 0.0) && (dv.y == 0.0)) {
		return 0.0;
	}

	// 角度計算
	std::double_t radian = atan2(dv.y, dv.x);
	std::double_t degree = convRadian2Degree(radian);
	degree = (degree < 0.0) ? (360.0 + degree) : degree;
	return degree;
}


//単位行列を取得
fw::MatrixF fw::Math::identifyMatrixF()
{
	MatrixF matrix = { 0.0F };
	matrix.mat[0][0] = 1.0F;
	matrix.mat[1][1] = 1.0F;
	matrix.mat[2][2] = 1.0F;
	matrix.mat[3][3] = 1.0F;
	return matrix;
}
fw::MatrixD fw::Math::identifyMatrixD()
{
	MatrixD matrix = { 0.0 };
	matrix.mat[0][0] = 1.0;
	matrix.mat[1][1] = 1.0;
	matrix.mat[2][2] = 1.0;
	matrix.mat[3][3] = 1.0;
	return matrix;
}

//行列の積
fw::MatrixD fw::Math::multiplyMatrix(const MatrixD& m1, const MatrixD& m2)
{
	MatrixD matrix = { 0.0 };
	for (int32_t row = 0; row < 4; row++) {			//行
		for (int32_t col = 0; col < 4; col++) {		//列
			matrix.mat[row][col] =
				(m1.mat[row][0] * m2.mat[0][col]) +
				(m1.mat[row][1] * m2.mat[1][col]) +
				(m1.mat[row][2] * m2.mat[2][col]) +
				(m1.mat[row][3] * m2.mat[3][col]);
		}
	}
	return matrix;
}

//行列の転置
fw::MatrixD fw::Math::transposeMatrix(const MatrixD& m)
{
	MatrixD matrix = { 0.0 };

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

//平行移動行列の生成
fw::MatrixD fw::Math::translateMatrix(const VectorD& trans)
{
	MatrixD matrix = { 0.0 };

	//単位行列の生成
	matrix = identifyMatrixD();
	
	//平行移動行列の生成
	matrix.mat[0][3] = trans.x;
	matrix.mat[1][3] = trans.y;
	matrix.mat[2][3] = trans.z;
	return matrix;
}

//回転行列の生成
fw::MatrixD fw::Math::rotateMatrix(const VectorD& rotate)
{
	//入力された回転角度を度からラジアンに変換
	VectorD rad = { 0.0 };
	rad.x = convDegree2Radian(rotate.x);
	rad.y = convDegree2Radian(rotate.y);
	rad.z = convDegree2Radian(rotate.z);

	//x方向の回転行列
	MatrixD matX = { 0.0 };
	matX = identifyMatrixD();
	if (rad.x != 0.0) {
		//コサイン,サインを計算
		std::double_t c = cos(rad.x);
		std::double_t s = sin(rad.x);

		matX.mat[1][1] = c;
		matX.mat[1][2] = -s;
		matX.mat[2][1] = s;
		matX.mat[2][2] = c;
	}

	//y方向の回転行列
	MatrixD matY = { 0.0 };
	matY = identifyMatrixD();
	if (rad.y != 0.0) {
		//コサイン,サインを計算
		std::double_t c = cos(rad.y);
		std::double_t s = sin(rad.y);

		matY.mat[0][0] = c;
		matY.mat[0][2] = s;
		matY.mat[2][0] = -s;
		matY.mat[2][2] = c;
	}

	//z方向の回転行列
	MatrixD matZ = { 0.0 };
	matZ = identifyMatrixD();
	if (rad.z != 0.0) {
		//コサイン,サインを計算
		std::double_t c = cos(rad.z);
		std::double_t s = sin(rad.z);

		matZ.mat[0][0] = c;
		matZ.mat[0][1] = -s;
		matZ.mat[1][0] = s;
		matZ.mat[1][1] = c;
	}

	//XYZ方向の行列の積から回転行列を生成
	MatrixD matrix = { 0.0 };
	matrix = multiplyMatrix(matX, matY);
	matrix = multiplyMatrix(matrix, matZ);
	return matrix;
}

//拡大縮小行列の生成
fw::MatrixD fw::Math::scaleMatrix(const VectorD& scale)
{
	MatrixD matrix = { 0.0 };

	//単位行列の生成
	matrix = identifyMatrixD();

	//拡大縮小行列の生成
	matrix.mat[0][0] = scale.x;
	matrix.mat[1][1] = scale.y;
	matrix.mat[2][2] = scale.z;
	return matrix;
}

//モデル変換行列の生成
fw::MatrixD fw::Math::modelMatrix(const VectorD& trans, const VectorD& rotate, const VectorD& scale)
{
	//拡大縮小変換行列を生成
	MatrixD matScale = { 0.0 };
	matScale = scaleMatrix(scale);

	//回転変換行列を生成
	MatrixD matRotate = { 0.0 };
	matRotate = rotateMatrix(rotate);

	// 平行移動変換行列を生成
	MatrixD matTranslate = { 0.0 };
	matTranslate = translateMatrix(trans);

	// モデル変換行列を生成
	MatrixD matrix = { 0.0 };
	matrix = multiplyMatrix(matScale, matRotate);
	matrix = multiplyMatrix(matrix, matTranslate);
	return matrix;
}

//カメラ視野行列の生成(orthogonalMatrix)
//left		垂直座標(左)
//right		垂直座標(右)
//bottom	水平座標(下)
//top		水平座標(上)
//znear		Z位置(手前)
//zfar		Z位置(奥)
fw::MatrixD fw::Math::orthogonalMatrix(const std::double_t left, const std::double_t right, const std::double_t bottom, const std::double_t top, const std::double_t znear, const std::double_t zfar)
{
	MatrixD matrix = { 0.0 };

	//1行目
	matrix.mat[0][0] = 2.0 / (right - left);
	matrix.mat[0][1] = 0.0;
	matrix.mat[0][2] = 0.0;
	matrix.mat[0][3] = -(right + left) / (right - left);

	//2行目
	matrix.mat[1][0] = 0.0;
	matrix.mat[1][1] = 2.0 / (top - bottom);
	matrix.mat[1][2] = 0.0;
	matrix.mat[1][3] = -(top + bottom) / (top - bottom);

	//3行目
	matrix.mat[2][0] = 0.0;
	matrix.mat[2][1] = 0.0;
	matrix.mat[2][2] = -2.0 / (zfar - znear);
	matrix.mat[2][3] = -(zfar + znear) / (zfar - znear);

	//4行目
	matrix.mat[3][0] = 0.0;
	matrix.mat[3][1] = 0.0;
	matrix.mat[3][2] = 0.0;
	matrix.mat[3][3] = 1.0;

	return matrix;
}
//カメラ視野行列の生成(frustum)
//left		垂直座標(左)
//right		垂直座標(右)
//bottom	水平座標(下)
//top		水平座標(上)
//znear		Z位置(手前)
//zfar		Z位置(奥)
fw::MatrixD fw::Math::frustumMatrix(const std::double_t left, const std::double_t right, const std::double_t bottom, const std::double_t top, const std::double_t znear, const std::double_t zfar)
{
	MatrixD matrix = { 0.0 };

	matrix.mat[0][0] = (2.0 * znear) / (right - left);
	matrix.mat[0][1] = 0.0;
	matrix.mat[0][2] = (right + left) / (right - left);
	matrix.mat[0][3] = 0.0;

	matrix.mat[1][0] = 0.0;
	matrix.mat[1][1] = (2.0 * znear) / (top - bottom);
	matrix.mat[1][2] = (top + bottom) / (top - bottom);
	matrix.mat[1][3] = 0.0;

	matrix.mat[2][0] = 0.0;
	matrix.mat[2][1] = 0.0;
	matrix.mat[2][2] = -(zfar + znear) / (zfar - znear);
	matrix.mat[2][3] = -(2.0 * zfar * znear) / (zfar - znear);

	matrix.mat[3][0] = 0.0;
	matrix.mat[3][1] = 0.0;
	matrix.mat[3][2] = -1.0;
	matrix.mat[3][3] = 0.0;

	return matrix;
}
//カメラ視野行列の生成(perspective)
//fovy		縦の視野角(度)
//aspect	横の視野角(倍率)
//znear		Z位置(手前)
//zfar		Z位置(奥)
fw::MatrixD fw::Math::perspectiveMatrix(const std::double_t fovy, const std::double_t aspect, const std::double_t znear, const std::double_t zfar)
{
	//縦の視野角を度からラジアンに変換
	std::double_t radFovy = convDegree2Radian(fovy);
	std::double_t f = 1.0 / (tan(radFovy) / 2.0);

	MatrixD matrix = { 0.0 };

	matrix.mat[0][0] = f / aspect;
	matrix.mat[0][1] = 0.0;
	matrix.mat[0][2] = 0.0;
	matrix.mat[0][3] = 0.0;

	matrix.mat[1][0] = 0.0;
	matrix.mat[1][1] = f;
	matrix.mat[1][2] = 0.0;
	matrix.mat[1][3] = 0.0;

	matrix.mat[2][0] = 0.0;
	matrix.mat[2][1] = 0.0;
	matrix.mat[2][2] = (znear + zfar) / (znear - zfar);
	matrix.mat[2][3] = (2.0 * znear * zfar) / (znear - zfar);

	matrix.mat[3][0] = 0.0;
	matrix.mat[3][1] = 0.0;
	matrix.mat[3][2] = -1.0;
	matrix.mat[3][3] = 0.0;

	return matrix;
}

//カメラ視点行列の生成(lookAt)
//eye	視点
//look	注視点
//up	向き
fw::MatrixD fw::Math::lookMatrix(const VectorD& eye, const VectorD& look, const VectorD& up)
{
	//カメラ前方向ベクトル(Z軸)
	VectorD forwardVector = { (look.x - eye.x), (look.y - eye.y), (look.z - eye.z) };
	forwardVector = normalizeVector(forwardVector);

	//カメラ横方向ベクトル(前方向ベクトルと向きベクトルの外積)(X軸)
	VectorD sideVector= crossVector(forwardVector, up);
	sideVector = normalizeVector(sideVector);

	//カメラ上方向ベクトル(横方向ベクトルと前方向ベクトルの外積)(Y軸)
	VectorD upVector = crossVector(sideVector, forwardVector);
	upVector = normalizeVector(upVector);

	MatrixD matrix = { 0.0 };

	matrix.mat[0][0] = sideVector.x;
	matrix.mat[0][1] = sideVector.y;
	matrix.mat[0][2] = sideVector.z;
	matrix.mat[0][3] = -dotVector(sideVector, eye);

	matrix.mat[1][0] = upVector.x;
	matrix.mat[1][1] = upVector.y;
	matrix.mat[1][2] = upVector.z;
	matrix.mat[1][3] = -dotVector(upVector, eye);

	matrix.mat[2][0] = -forwardVector.x;
	matrix.mat[2][1] = -forwardVector.y;
	matrix.mat[2][2] = -forwardVector.z;
	matrix.mat[2][3] = dotVector(forwardVector, eye);

	matrix.mat[3][0] = 0.0;
	matrix.mat[3][1] = 0.0;
	matrix.mat[3][2] = 0.0;
	matrix.mat[3][3] = 1.0;

	return matrix;
}