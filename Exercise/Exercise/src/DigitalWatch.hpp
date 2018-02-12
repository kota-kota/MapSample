#ifndef INCLUDED_DIGITALWATCH_HPP
#define INCLUDED_DIGITALWATCH_HPP

#include "Type.hpp"
#include <Windows.h>

namespace ui {

	//前方宣言
	class DrawWGL;

	//----------------------------------------------------------
	//
	// デジタル時計クラス定義
	//
	//----------------------------------------------------------

	class DigitalWatch {
		//メンバ変数
		HWND			hWnd_;
		ui::DrawWGL*	drawIF_;

	public:
		//コンストラクタ
		DigitalWatch(const HWND hWnd);
		//デストラクタ
		~DigitalWatch();
		//描画
		void draw();
	};
}

#endif //INCLUDED_DIGITALWATCH_HPP
