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
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

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
    private int mPositionHandle;
    private int mPotentialHandle;
    private final int mSynapseProgram;

    private FloatBuffer synConnectionBuffer;
    private FloatBuffer synPotentialBuffer;

    public BrainDrawer (Context context){
        this.context = context;

        String vertexShaderCode = readFileAsString("synapse.shader");
        String fragmentShaderCode = readFileAsString("synapse.Fshader");

        // prepare shaders and OpenGL program
        int vertexShader = MyGLRenderer.loadShader(
                GLES30.GL_VERTEX_SHADER, vertexShaderCode);
        checkGlError("Load synapse vertex shader");
        int fragmentShader = MyGLRenderer.loadShader(
                GLES30.GL_FRAGMENT_SHADER, fragmentShaderCode);
        checkGlError("Load synapse fragment shader");

        mSynapseProgram = GLES30.glCreateProgram();             // create empty OpenGL Program
        checkGlError("Create program");
        GLES30.glAttachShader(mSynapseProgram, vertexShader);   // add the vertex shader to program
        checkGlError("Attach synapse vertex shader");
        GLES30.glAttachShader(mSynapseProgram, fragmentShader); // add the fragment shader to program
        checkGlError("Attach synapse fragment shader");
        GLES30.glLinkProgram(mSynapseProgram);                  // create OpenGL program executables
        checkGlError("Link synapse Program");

        mPotentialHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPotential");

    }

    public void draw(float[] mvpMatrix) {
        getRenderData();
        float[] connections = getSynConnections();
        float[] potentials = getPotentials();

        Log.i("Connections","Length "+connections.length+", array: "+connections.toString());
        Log.i("Potentials", "Length "+potentials.length +", array: "+potentials.toString());

        GLES30.glLineWidth(2.5f);
        // Add program to OpenGL environment
        GLES30.glUseProgram(mSynapseProgram);


        // initialize vertex byte buffer for shape coordinates
        ByteBuffer bbConnections = ByteBuffer.allocateDirect(
                // (number of coordinate values * 4 bytes per float)
                connections.length * 4);
        // use the device hardware's native byte order
        bbConnections.order(ByteOrder.nativeOrder());

        // create a floating point buffer from the ByteBuffer
        synConnectionBuffer = bbConnections.asFloatBuffer();
        // add the coordinates to the FloatBuffer
        synConnectionBuffer.put(connections);
        // set the buffer to read the first coordinate
        synConnectionBuffer.position(0);

        // get handle to vertex shader's vPosition member
        mPositionHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPosition");

        GLES30.glVertexAttribPointer(
                mPositionHandle, 3,
                GLES30.GL_FLOAT, false,
                3*4, synConnectionBuffer);


        // initialize vertex byte buffer for shape coordinates
        ByteBuffer bbPotential = ByteBuffer.allocateDirect(
                // (number of coordinate values * 4 bytes per float)
                potentials.length * 4);
        // use the device hardware's native byte order
        bbPotential.order(ByteOrder.nativeOrder());

        // create a floating point buffer from the ByteBuffer
        synPotentialBuffer = bbPotential.asFloatBuffer();
        // add the coordinates to the FloatBuffer
        synPotentialBuffer.put(potentials);
        // set the buffer to read the first coordinate
        synPotentialBuffer.position(0);

        mPotentialHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPotential");
        if (mPositionHandle == -1){
            Log.e("Shader error","Did not find vPotential attribute");
        }

        GLES30.glVertexAttribPointer(
                mPotentialHandle, 1,
                GLES30.GL_FLOAT, false,
                1*4, synPotentialBuffer);

        // get handle to shape's transformation matrix

        mMVPMatrixHandle = GLES30.glGetUniformLocation(mSynapseProgram, "uMVPMatrix");
        MyGLRenderer.checkGlError("glGetUniformLocation");

        // Apply the projection and view transformation
        GLES30.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        MyGLRenderer.checkGlError("glUniformMatrix4fv");


        GLES30.glDrawArrays(GLES30.GL_LINES, 0, potentials.length);

        GLES30.glDisableVertexAttribArray(mPositionHandle);
        GLES30.glDisableVertexAttribArray(mPotentialHandle);

    }

    private String readFileAsString(String filePath) {
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
