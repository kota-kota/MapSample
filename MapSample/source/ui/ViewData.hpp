#ifndef INCLUDED_UIDRAW_HPP
#define INCLUDED_UIDRAW_HPP

#include "Std.hpp"
#include "UiDef.hpp"
#include "image/Image.hpp"
#include <string>
#include <vector>

namespace fw {
	//前方宣言
	class DrawIF;
}

namespace ui {

	//----------------------------------------------------------
	//
	// 各描画物定数定義
	//
	//----------------------------------------------------------

	//型定義
	using DrawColors = std::vector<std::Color>;
	using DrawCoords = std::vector<std::CoordI>;

	//描画物タイプ
	enum EN_DrawPartsType {
		POINT,
		LINE,
		POLYGON,
		IMAGE,
		STRING,
	};


	//----------------------------------------------------------
	//
	// 表示物(基底)クラス
	//
	//----------------------------------------------------------
	class ViewParts {
	public:
		//描画
		virtual void draw(fw::DrawIF* const drawIF) = 0;
	};


	//----------------------------------------------------------
	//
	// 表示物(点)クラス
	//
	//----------------------------------------------------------
	class ViewPoint : public ViewParts {
		//メンバ変数
		DrawCoords		coords_;	//座標
		DrawColors		colors_;	//色
		std::float_t	width_;		//点幅

	public:
		//コンストラクタ
		ViewPoint(const DrawCoords& coords, const DrawColors& colors, const float_t width);
		//描画
		virtual void draw(fw::DrawIF* const drawIF);
	};


	//----------------------------------------------------------
	//
	// 表示物(線)クラス
	//
	//----------------------------------------------------------
	class ViewLine : public ViewParts {
		//メンバ変数
		DrawCoords		coords_;	//座標
		DrawColors		colors_;	//色
		std::float_t	width_;		//線幅

	public:
		//コンストラクタ
		ViewLine(const DrawCoords& coords, const DrawColors& colors, const float_t width);
		//描画
		virtual void draw(fw::DrawIF* const drawIF);
	};


	//----------------------------------------------------------
	//
	// 表示物(ポリゴン)クラス
	//
	//----------------------------------------------------------
	class ViewPolygon : public ViewParts {
		//メンバ変数
		DrawCoords		coords_;	//座標
		DrawColors		colors_;	//色

	public:
		//コンストラクタ
		ViewPolygon(const DrawCoords& coords, const DrawColors& colors);
		//描画
		virtual void draw(fw::DrawIF* const drawIF);
	};


	//----------------------------------------------------------
	//
	// 表示物(イメージ)クラス
	//
	//----------------------------------------------------------
	class ViewImage : public ViewParts {
		//メンバ変数
		std::CoordI	coord_;	//座標
		fw::Image	image_;	//イメージ

	public:
		//コンストラクタ
		ViewImage(const std::CoordI& coord, const fw::Image& image);
		//描画
		virtual void draw(fw::DrawIF* const drawIF);
	};


	//----------------------------------------------------------
	//
	// 表示物(文字列)クラス
	//
	//----------------------------------------------------------
	class ViewString : public ViewParts {
		//メンバ変数
		std::CoordI		coord_;	//座標
		std::wstring	str_;	//文字列

	public:
		//コンストラクタ
		ViewString(const std::CoordI& coord, const std::wstring& str);
		//描画
		virtual void draw(fw::DrawIF* const drawIF);
	};



	//----------------------------------------------------------
	//
	// 表示データクラス
	//
	//----------------------------------------------------------

	class ViewData {
		//メンバ変数
		std::Color					backColor_;	//背景色
		std::vector<ViewParts*>		partsList_;	//描画物リスト

	public:
		//コンストラクタ
		ViewData();
		//デストラクタ
		~ViewData();
		//背景色設定
		void setBackColor(const std::Color& backColor);
		//描画物設定
		void setDrawParts(ViewParts* const parts);
		//描画
		void draw(fw::DrawIF* const drawIF, std::CoordI& mapPos);
	};
}

#endif //INCLUDED_UIDRAW_HPP
