#ifndef INCLUDED_WINDOWIF_HPP
#define INCLUDED_WINDOWIF_HPP

#include "Type.hpp"

namespace window {

	/**
	 * ウィンドウ操作インターフェースクラス
	 */
	class WindowIF {
	public:
		//デストラクタ
		virtual ~WindowIF() {}
		//ウィンドウサイズ取得
		virtual void getWindowSize(std::int32_t* const width, std::int32_t* const height) = 0;
		//描画カレント
		virtual void makeCurrent(const bool current) = 0;
		//描画更新
		virtual void swapBuffers() = 0;
	};
}

#endif //INCLUDED_WINDOWIF_HPP
