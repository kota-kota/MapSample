#ifndef INCLUDED_DRAWIF_HPP
#define INCLUDED_DRAWIF_HPP

#include "Std.hpp"
#include "Math.hpp"
#include <vector>

#define DRAWIF_WGL
//#define DRAWIF_WEGL

namespace fw {
	//前方宣言
	struct Image;
	class Font;
}

namespace fw {
	
	//描画用配列
	using DrawCoords = std::vector<std::CoordI>;
	using DrawColors = std::vector<std::ColorUB>;

	//----------------------------------------------------------
	//
	// 描画ステータス構造体
	//
	//----------------------------------------------------------

	struct DrawStatus {
		std::AreaI		viewport_;
		fw::VectorD		viewEye_;
		fw::VectorD		viewLook_;
		fw::VectorD		viewUp_;
		fw::MatrixD		viewMat_;
		fw::MatrixD		projMat_;
	};


	//----------------------------------------------------------
	//
	// 描画インターフェースクラス
	//
	//----------------------------------------------------------

	class DrawIF {
	protected:
		fw::Font*		font_;		//フォント

	public:
		//コンストラクタ
		DrawIF();
		//デストラクタ
		virtual ~DrawIF();
		//作成
		virtual void create() = 0;
		//セットアップ
		virtual void setup(const DrawStatus& drawStatus) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
		//クリア
		virtual void clear(const std::ColorUB& color) = 0;
		//点描画
		virtual void drawPoints(const DrawCoords& coords, const DrawColors& colors, const std::float_t size) = 0;
		//ライン描画
		virtual void drawLines(const DrawCoords& coords, const DrawColors& colors, const std::float_t width) = 0;
		//ポリゴン描画
		virtual void drawPolygons(const DrawCoords& coords, const DrawColors& colors) = 0;
		//イメージ描画
		virtual void drawImage(const std::CoordI& coord, const fw::Image& image) = 0;
		//文字描画
		virtual void drawString(const std::CoordI& coord, const wchar_t* const str) = 0;

		//画面幅高さを取得
		virtual std::WH getScreenWH() = 0;
	};
}

#endif //INCLUDED_DRAWIF_HPP
