#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>

namespace fw {
    class SampleMain {
        void*           nativeWindow_;
        EGLDisplay		eglDpy_;
        EGLConfig		eglCfg_;
        EGLSurface		eglWin_;
        EGLContext		eglCtx_;

    public:
        //コンストラクタ
        SampleMain();
        //デストラクタ
        ~SampleMain();
        //サーフェイス作成
        bool createSurface(void* nativeWindow);
        //サーフェイス破棄
        void destroySurface();
        //描画
        void draw();
    };
}