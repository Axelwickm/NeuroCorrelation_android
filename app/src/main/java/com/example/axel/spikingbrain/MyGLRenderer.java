package com.example.axel.spikingbrain;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.example.axel.spikingbrain.LibJNIWrapper.getMVPMatrix;
import static com.example.axel.spikingbrain.LibJNIWrapper.runBrain;

public class MyGLRenderer implements GLSurfaceView.Renderer {
    private Context context;
    private BrainDrawer mBrain;

    // Mvp-matris (omvandlar hörnens-globala variabler till skrämkoordinater genom att multiplicera)
    private float[] mMVPMatrix = new float[16];

    // Bildförhållande
    private float ratio;

    // Konstruktorn sätter bara kontexten
    MyGLRenderer (Context context){
        this.context = context;
    }

    // Kompilerar shaders
    public static int loadShader(int type, String shaderCode) {
        // Skapar shader av given typ (Vertex eller Fragment)
        int shader = GLES30.glCreateShader(type);

        // Ger OpenGL koden, och skapar ett handle
        GLES30.glShaderSource(shader, shaderCode);
        GLES30.glCompileShader(shader);

        return shader; // Handle returneras
    }

    public static void checkGlError(String glOperation) {
        int error;
        while ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            Log.e("OpenGL error", glOperation + ": glError " + error);
            throw new RuntimeException(glOperation + ": glError " + error);
        }
    }

    @Override
    // När surfacen skapas
    public void onSurfaceCreated(GL10 unused, EGLConfig config) {
        // Sätter bakgrundsfärg
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Tillåter transparans (och säger hur den beräknas)
        GLES30.glEnable(GLES30.GL_BLEND);
        GLES30.glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_ONE_MINUS_SRC_ALPHA);

        // Skapar hjärn-ritaren
        mBrain = new BrainDrawer(context);
    }

    @Override
    // Varje gång hjärnan ska uppdateras
    public void onDrawFrame(GL10 unused) {
        // Målar över skärmen
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);

        // Hämtar MVP-matriesn från LibJNIWrapper (beräknas i NeuCor_Renderer i C++)
        mMVPMatrix = getMVPMatrix(ratio);

        // Ritar ut hjärnan på skrämen
        mBrain.draw(mMVPMatrix);

        // Simulerar hjärnan
        runBrain();
    }

    @Override
    // När surface ändras
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        // Rätar ut och ändrar bildförhållande
        GLES30.glViewport(0, 0, width, height);
        ratio = (float) width / height;
    }

}