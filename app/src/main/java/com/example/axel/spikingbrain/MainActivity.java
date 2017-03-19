package com.example.axel.spikingbrain;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import static com.example.axel.spikingbrain.LibJNIWrapper.init;

public class MainActivity extends AppCompatActivity {
    GLSurfaceView glSurfaceView; // Surface view som OpenGl använder
    private boolean rendererSet; // Ser till att inte göra nått dumt om inte renderaren är fixad än

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initerar LibJNIWrapper
        init();

        glSurfaceView = new GLSurfaceView(this);

        glSurfaceView.setEGLContextClientVersion(3); // Använd OpenGL 3
        glSurfaceView.setRenderer(new MyGLRenderer(getBaseContext())); // Sätter renderar-klassen som renderare
        rendererSet = true; // Nu är det inte dumt att använda renderaren längre
        setContentView(glSurfaceView);

    }

    @Override
    protected void onPause() {
        super.onPause();

        if (rendererSet) {
            glSurfaceView.onPause(); // Pausar
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (rendererSet) {
            glSurfaceView.onResume(); // Unpausar
        }
    }
}

