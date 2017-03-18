package com.example.axel.spikingbrain;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES30;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.nio.FloatBuffer;

import static com.example.axel.spikingbrain.LibJNIWrapper.getPotentials;
import static com.example.axel.spikingbrain.LibJNIWrapper.getRenderData;
import static com.example.axel.spikingbrain.LibJNIWrapper.getSynConnections;
import static com.example.axel.spikingbrain.MyGLRenderer.checkGlError;


/**
 * Created by aw980204 on 2017-03-09.
 */

public class BrainDrawer {
    //private final FloatBuffer vertexBuffer;
    //private final int synapseProgram;
    private Context context;

    private int mMVPMatrixHandle;
    private final int mSynapseProgram;

    public BrainDrawer (Context context){
        this.context = context;

        final String vertexShaderCode = readFileAsString("synapse.shader");
        final String fragmentShaderCode = readFileAsString("synapse.Fshader");

        // prepare shaders and OpenGL program
        int vertexShader = MyGLRenderer.loadShader(
                GLES30.GL_VERTEX_SHADER, vertexShaderCode);
        checkGlError("Load synapse vertex shader");
        int fragmentShader = MyGLRenderer.loadShader(
                GLES30.GL_FRAGMENT_SHADER, fragmentShaderCode);
        checkGlError("Load synapse fragment shader");

        mSynapseProgram = GLES30.glCreateProgram();             // create empty OpenGL Program
        GLES30.glAttachShader(mSynapseProgram, vertexShader);   // add the vertex shader to program
        checkGlError("Attach synapse vertex shader");
        GLES30.glAttachShader(mSynapseProgram, fragmentShader); // add the fragment shader to program
        checkGlError("Attach synapse fragment shader");
        GLES30.glLinkProgram(mSynapseProgram);                  // create OpenGL program executables
        checkGlError("Link synapse Program");
    }

    public void draw(float[] mvpMatrix) {
        getRenderData();
        float[] connections = getSynConnections();
        float[] potentials = getPotentials();
    }

    public String readFileAsString(String filePath) {
        InputStream input;
        String text = "";

        try {
            input = context.getAssets().open(filePath);

            int size = input.available();
            byte[] buffer = new byte[size];
            input.read(buffer);
            input.close();

            // byte buffer into a string
            text = new String(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }

        return text;
    }

}
