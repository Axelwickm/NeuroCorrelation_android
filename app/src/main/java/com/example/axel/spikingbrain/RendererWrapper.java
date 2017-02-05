package com.example.axel.spikingbrain;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements GLSurfaceView.Renderer {
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        LibJNIWrapper.on_surface_created();
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