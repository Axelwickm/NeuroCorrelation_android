package com.example.axel.spikingbrain;

import android.content.res.AssetManager;

/**
 * Created by Axel on 2017-02-04.
 */

public class LibJNIWrapper {
    static {
        System.loadLibrary("native-lib");
    }

    public static native void init();

    public static native void runBrain();

    public static native void getRenderData();

    public static native float[] getSynConnections();

    public static native float[] getPotentials();
}