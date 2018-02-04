#ifndef INCLUDED_DRAWWGLES_HPP
#define INCLUDED_DRAWWGLES_HPP

#include "DrawIF.hpp"

#ifdef DRAWIF_WEGL
#include <Windows.h>

namespace image {
	//前方宣言
	class Font;
}

namespace fw {

	//シェーダタイプ
	enum EN_ShaderType {
		COLOR_RGBA,			//カラー(RGBA)
		COLOR_LINE_RGBA,	//カラーライン(RGBA)
		TEXTURE_RGBA,		//テクスチャ(RGBA)
	};

	//シェーダパラメータ(カラーRGBA)
	struct ShaderPara_ColorRGBA {
		uint32_t	progId_;
		uint32_t	attr_point_;
		uint32_t	attr_color_;
		uint32_t	unif_model_;
		uint32_t	unif_view_;
		uint32_t	unif_proj_;
	};

	//シェーダパラメータ
	struct ShaderPara {
		ShaderPara_ColorRGBA	colorRGBA_;	//カラーRGBA
	};

	//----------------------------------------------------------
	//
	// WEGL描画クラス
	//
	//----------------------------------------------------------

	class DrawWEGL : public DrawIF {
		//メンバ変数
		HWND			hWnd_;
		void*			display_;
		void*			config_;
		void*			surface_;
		void*			context_;
		image::Font*	font_;

		fw::MatrixF		view_;	//ビュー変換行列
		fw::MatrixF		proj_;	//プロジェクション変換行列

		ShaderPara		shaderPara_;

	public:
		//コンストラクタ
		DrawWEGL(const HWND hWnd = nullptr);
		//デストラクタ
		virtual ~DrawWEGL();
		//作成
		virtual void create();
		//セットアップ
		virtual void setup(const std::Position& mapPos);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const std::ColorUB& color);
		//点描画
		virtual void drawPoints(const DrawCoords& coords, const DrawColors& colors, const std::float_t size);
		//ライン描画
		virtual void drawLines(const DrawCoords& coords, const DrawColors& colors, const std::float_t width);
		//ポリゴン描画
		virtual void drawPolygons(const DrawCoords& coords, const DrawColors& colors);
		//イメージ描画
		virtual void drawImage(const std::CoordI& coord, const fw::Image& image);
		//文字描画
		virtual void drawString(const std::CoordI& coord, const wchar_t* const str);

		//画面幅高さを取得
		virtual std::WH getScreenWH();
	};
}

#endif //DRAWIF_WEGL
#endif //INCLUDED_DRAWWGLES_HPP
