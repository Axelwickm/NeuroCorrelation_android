package com.example.axel.spikingbrain;

/**
 * Created by Axel on 2017-02-04.
 */

public class LibJNIWrapper {
    static {
        System.loadLibrary("native-lib");
    }

    public static native void on_surface_created();

    public static native void on_surface_changed(int width, int height);

    public static native void on_draw_frame();
}