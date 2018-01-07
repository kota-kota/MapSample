#ifndef INCLUDED_VIEWDATA_HPP
#define INCLUDED_VIEWDATA_HPP

#include "Std.hpp"
#include "UiDef.hpp"
#include "draw/DrawIF.hpp"
#include "image/Image.hpp"
#include <string>
#include <vector>


namespace ui {

	//----------------------------------------------------------
	//
	// 各描画物定数定義
	//
	//----------------------------------------------------------

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
		fw::DrawCoords	coords_;	//座標
		fw::DrawColors	colors_;	//色
		std::float_t	width_;		//点幅

	public:
		//コンストラクタ
		ViewPoint(const fw::DrawCoords& coords, const fw::DrawColors& colors, const std::float_t width);
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
		fw::DrawCoords	coords_;	//座標
		fw::DrawColors	colors_;	//色
		std::float_t	width_;		//線幅

	public:
		//コンストラクタ
		ViewLine(const fw::DrawCoords& coords, const fw::DrawColors& colors, const std::float_t width);
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
		fw::DrawCoords	coords_;	//座標
		fw::DrawColors	colors_;	//色

	public:
		//コンストラクタ
		ViewPolygon(const fw::DrawCoords& coords, const fw::DrawColors& colors);
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
		std::ColorUB				backColor_;	//背景色
		std::vector<ViewParts*>		partsList_;	//描画物リスト

	public:
		//コンストラクタ
		ViewData();
		//デストラクタ
		~ViewData();
		//背景色設定
		void setBackColor(const std::ColorUB& backColor);
		//描画物設定
		void setDrawParts(ViewParts* const parts);
		//描画
		void draw(fw::DrawIF* const drawIF, const std::Position& mapPos);
	};
}

#endif //INCLUDED_VIEWDATA_HPP
