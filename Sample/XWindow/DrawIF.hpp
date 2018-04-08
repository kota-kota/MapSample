#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "Type.hpp"

namespace draw {

	//3次元ベクトル
	struct VectorF {
		std::float32_t	x;
		std::float32_t	y;
		std::float32_t	z;
	};

	//4*4行列
	struct MatrixF {
		std::float32_t	mat[4][4];	//[行][列]
	};

	struct PointF {
		std::float32_t	x;
		std::float32_t	y;
		std::float32_t	z;
	};

	struct ColorUB {
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
		std::uint8_t	a;
	};

	struct AreaI {
		std::int32_t	xmin;
		std::int32_t	ymin;
		std::int32_t	xmax;
		std::int32_t	ymax;
	};

	/**
	 * 描画インターフェースクラス
	 */
	class DrawIF {
	public:
		//デストラクタ
		virtual ~DrawIF() {}
		//セットアップ
		virtual void setup(const AreaI& area) = 0;
		//クリア
		virtual void clear(const ColorUB& color) = 0;
		//ライン描画
		virtual void drawLines(const int32_t pointNum, PointF* const points, ColorUB* colors, const std::float32_t width) = 0;

		//行列の転置
		static MatrixF transMatrix(const MatrixF& m);
		//プロジェクション行列生成(orthogonal)
		//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
		static MatrixF makeProjMatrix_Ortho(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar);
		//プロジェクション行列生成(frustum)
		//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
		static MatrixF makeProjMatrix_Frustum(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar);
	};
}

#endif //INCLUDED_DRAWIF_HPP
