#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "Type.hpp"

namespace fw {

	//前方宣言
	class Str;

	//3次元ベクトル
	struct VectorF {
		std::float32_t	x;
		std::float32_t	y;
		std::float32_t	z;
	};

	//4*4行列
	struct MatrixF {
		std::float32_t	mat[16];	//行1[0:3] 行2[4:7] 行3[8:11] 行4[12:15]
	};

	//描画座標
	struct PointF {
		std::float32_t	x;
		std::float32_t	y;
		std::float32_t	z;
	};

	//描画色
	struct ColorUB {
		std::uint8_t	r;
		std::uint8_t	g;
		std::uint8_t	b;
		std::uint8_t	a;
	};

	//エリア
	struct AreaI {
		std::int32_t	xmin;
		std::int32_t	ymin;
		std::int32_t	xmax;
		std::int32_t	ymax;
	};
	struct AreaF {
		std::float32_t	xmin;
		std::float32_t	ymin;
		std::float32_t	xmax;
		std::float32_t	ymax;
	};

	//ライン描画種別
	enum EN_LineType {
		LINES,
		LINE_STRIP,
		LINE_LOOP,
	};

	//ポリゴン描画種別
	enum EN_PolygonType {
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
	};

	//画像ピクセル形式
	enum EN_PixelFormat {
		A,
		RGB,
		RGBA,
	};

	//基準位置(X位置_Y位置)
	enum EN_BaseLoc {
		LEFT_TOP,
		LEFT_CENTER,
		LEFT_BOTTOM,
		CENTER_TOP,
		CENTER_CENTER,
		CENTER_BOTTOM,
		RIGHT_TOP,
		RIGHT_CENTER,
		RIGHT_BOTTOM,
	};

	//画像描画属性
	struct ImageAttr {
		std::uint32_t	id;
		std::int32_t	width;
		std::int32_t	height;
		EN_PixelFormat	pixFormat;
		EN_BaseLoc		baseLoc;
	};

	//テキスト描画属性
	struct TextAttr {
		std::int32_t	size;
		std::uint8_t	isBold;
		std::uint8_t	isBorder;
		ColorUB			bodyColor;
		ColorUB			borderColor;
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
		virtual void drawLines(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const std::float32_t width, const EN_LineType type) = 0;
		//ポリゴン描画
		virtual void drawPolygons(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const EN_PolygonType type) = 0;
		//画像描画
		virtual void drawImage(std::float32_t* const point, const std::float32_t angle, std::uint8_t* const image, const ImageAttr& imgAttr) = 0;
		//テキスト描画
		virtual void drawText(std::float32_t* const point, const Str& text, const TextAttr& textAttr) = 0;

		//ラジアンから角度に変換
		static inline std::float32_t convRadian2Degree(const std::float32_t rad);
		//角度からラジアンに変換
		static inline std::float32_t convDegree2Radian(const std::float32_t deg);
		//単位行列の生成
		static MatrixF identifyMatrixF();
		//行列の積
		static MatrixF multiplyMatrix(const MatrixF& m1, const MatrixF& m2);
		//行列の転置
		static MatrixF transposeMatrix(const MatrixF& m);
		//平行移動行列の生成
		static MatrixF translateMatrix(const VectorF& trans);
		//回転行列の生成
		static MatrixF rotateMatrix(const VectorF& rotate);
		//拡大縮小行列の生成
		static MatrixF scaleMatrix(const VectorF& scale);
		//モデル変換行列の生成
		static MatrixF modelMatrix(const VectorF& trans, const VectorF& rotate, const VectorF& scale);
		//プロジェクション行列生成(orthogonal)
		//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
		static MatrixF makeProjMatrix_Ortho(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar);
		//プロジェクション行列生成(frustum)
		//垂直座標(左),垂直座標(右),水平座標(下),水平座標(上),Z位置(手前),Z位置(奥)
		static MatrixF makeProjMatrix_Frustum(const std::float32_t left, const std::float32_t right, const std::float32_t bottom, const std::float32_t top, const std::float32_t znear, const std::float32_t zfar);
	};
}

#endif //INCLUDED_DRAWIF_HPP
