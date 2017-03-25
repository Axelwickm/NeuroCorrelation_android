package com.example.axel.spikingbrain;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import static com.example.axel.spikingbrain.LibJNIWrapper.touchEventCallback;

/**
 * Created by Axel on 2017-03-25.
 */

public class MyGLSurfaceView extends GLSurfaceView {

    private final float TOUCH_SCALE_FACTOR = 0.35f;
    private float mPreviousX;
    private float mPreviousY;

    public MyGLSurfaceView(Context context) {
        super(context);

        setEGLContextClientVersion(3); // Använd OpenGL 3
        setRenderer(new MyGLRenderer(context)); // Sätter renderar-klassen som renderare
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        float x = e.getX();
        float y = e.getY();

        switch (e.getAction()) {
            case MotionEvent.ACTION_MOVE:

                float dx = x - mPreviousX;
                float dy = y - mPreviousY;

                // reverse direction of rotation above the mid-line
                if (y > getHeight() / 2) {
                    dx = dx * -1 ;
                }

                // reverse direction of rotation to left of the mid-line
                if (x < getWidth() / 2) {
                    dy = dy * -1 ;
                }

                touchEventCallback(dx*TOUCH_SCALE_FACTOR, dy*TOUCH_SCALE_FACTOR);

        }

        mPreviousX = x;
        mPreviousY = y;
        return true;
    }

}
