#ifndef INCLUDED_LAYERMANAGER_HPP
#define INCLUDED_LAYERMANAGER_HPP

#include "LayerIF.hpp"

#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>

#include <list>

namespace window {

	/**
	 * レイヤー管理クラス
	 */
	class LayerManager {
		//メンバ変数
		void*			native_;
		EGLDisplay		eglDpy_;
		EGLConfig		eglCfg_;
		EGLSurface		eglWin_;
		EGLContext		eglCtx_;
		draw::DrawIF*	drawIF_;
		std::list<LayerIF*>	layerList_;

	public:
		//コンストラクタ
		LayerManager();
		//デストラクタ
		~LayerManager();
		//ウィンドウ作成
		bool createWindow(void* const native);
		//ウィンドウ破棄
		void destroyWindow();
		//レイヤー作成
		LayerIF* createLayer(const int32_t width, const int32_t height);
		//レイヤー破棄
		void destroyLayer(LayerIF** const layer);
	};
}

#endif //INCLUDED_LAYERMANAGER_HPP
