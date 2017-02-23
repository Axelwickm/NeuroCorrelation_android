package com.example.axel.spikingbrain;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements GLSurfaceView.Renderer {
    Context applicationContext;
    public RendererWrapper(Context applicationContext) {
        this.applicationContext = applicationContext;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        AssetManager assetManager = applicationContext.getAssets();
        LibJNIWrapper.on_surface_created(assetManager);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        LibJNIWrapper.on_surface_changed(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        LibJNIWrapper.on_draw_frame();
    }
}