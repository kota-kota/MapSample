package com.example.kyohei.androidwindow;

import android.view.Surface;

public class Jni {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native void nativeCreate();
    public static native void nativeDestroy();
    public static native void nativeCreateSurface(Surface surface, int w, int h);
    public static native void nativeDestroySurface();
    public static native void nativeDraw();
}
