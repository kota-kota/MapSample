#ifndef INCLUDED_DWUTILITY_HPP
#define INCLUDED_DWUTILITY_HPP

#include "DWType.hpp"
#include <Windows.h>
#include <time.h>
#include <mutex>

//OpenGL
#include <gl/GL.h>

//FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftsynth.h>
#include <freetype/ftglyph.h>

namespace dw {

	//DWWindowクラス
	class DWWindow {
		//フォント寸法情報
		struct FontMetrics {
			//メンバ変数
			std::int32_t	width_;		//フォント幅[pixel]
			std::int32_t	height_;	//フォント高さ[pixel]
			std::int32_t	offsetX_;	//グリフ原点(0,0)からグリフイメージの左端までの水平方向オフセット[pixel]
			std::int32_t	offsetY_;	//グリフ原点(0,0)からグリフイメージの上端までの垂直方向オフセット[pixel]
			std::int32_t	nextX_;		//次グリフへの水平方向オフセット[pixel]
			std::int32_t	nextY_;		//次グリフへの垂直方向オフセット[pixel]
			std::int32_t	kerningX_;	//水平方向カーニング
			std::int32_t	kerningY_;	//垂直方向カーニング
		};
		//フォントグリフ
		struct FontGlyph {
			FT_UInt			index_;		//グリフインデックス
			FT_Glyph		image_;		//グリフイメージ
			FontMetrics		metrics_;	//寸法情報
		};

		//メンバ変数
		HWND		hWnd_;
		HDC			hDC_;
		HGLRC		hGLRC_;

		FT_Library	ftLibrary_;
		FT_Face		ftFace_;

		DWSize		size_;

	public:
		//作成
		static void create(void* native);
		//取得
		static DWWindow* get();
		//破棄
		static void destroy();

		//描画開始
		void beginDraw();
		//描画終了
		void endDraw();
		//画面塗りつぶし
		void clear(const DWColor& color);
		//文字描画
		void drawText(const DWText& text);
		//画像描画
		void drawBitmap();

	private:
		//コンストラクタ
		DWWindow(HWND hWnd);
		//デストラクタ
		~DWWindow();
	};

	//DWFuncクラス
	class DWFunc {
	public:
		//時刻取得
		static DWTime getTime();
	};
};


#endif //INCLUDED_DWUTILITY_HPP
