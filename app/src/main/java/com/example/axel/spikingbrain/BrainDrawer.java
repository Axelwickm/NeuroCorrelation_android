/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.axel.spikingbrain;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.content.Context;
import android.opengl.GLES30;
import android.util.Log;

import static com.example.axel.spikingbrain.LibJNIWrapper.clearRenderData;
import static com.example.axel.spikingbrain.LibJNIWrapper.getRenderData;
import static com.example.axel.spikingbrain.LibJNIWrapper.getSynConnections;
import static com.example.axel.spikingbrain.LibJNIWrapper.getSynPotentials;
import static com.example.axel.spikingbrain.MyGLRenderer.checkGlError;

/**
 * A two-dimensional triangle for use as a drawn object in OpenGL ES 2.0.
 */
public class BrainDrawer {
    private Context context;

    private FloatBuffer synPosBuffer;
    private FloatBuffer synPotBuffer;
    private final int mSynapseProgram;
    private int mPositionHandle;
    private int mPotentialHandle;
    private int mMVPMatrixHandle;

    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 3;

    private int vertexCount;
    private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex

    float color[] = { 0.63671875f, 0.76953125f, 0.22265625f, 0.0f };

    public BrainDrawer(Context context) {
        // Sparar context
        this.context = context;

        getRenderData();
        updatePosBuffer(); // Hämtar positions-array endast en gång, då neuronernas position ej förändras
        clearRenderData();

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
        checkGlError("Link synapse Program");              // create OpenGL program executables

    }

    public void draw(float[] mvpMatrix) {
        getRenderData();
        updatePotBuffer(); // Hämtar synapsernas spännings-array
        clearRenderData();

        // Ändra linjetjockleken
        GLES30.glLineWidth(2.5f);
        // Add program to OpenGL environment
        GLES30.glUseProgram(mSynapseProgram);

        // get handle to vertex shader's vPosition member
        mPositionHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPosition");

        // Enable a handle to the triangle vertices
        GLES30.glEnableVertexAttribArray(mPositionHandle);

        // Prepare the triangle coordinate data
        GLES30.glVertexAttribPointer(
                mPositionHandle, COORDS_PER_VERTEX,
                GLES30.GL_FLOAT, false,
                vertexStride, synPosBuffer);


        // get handle to vertex shader's vPosition member
        mPotentialHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPotential");

        // Enable a handle to the triangle vertices
        GLES30.glEnableVertexAttribArray(mPotentialHandle);

        // Prepare the triangle coordinate data
        GLES30.glVertexAttribPointer(
                mPotentialHandle, 1,
                GLES30.GL_FLOAT, false,
                4, synPotBuffer);

        // get handle to shape's transformation matrix
        mMVPMatrixHandle = GLES30.glGetUniformLocation(mSynapseProgram, "uMVPMatrix");
        checkGlError("glGetUniformLocation");

        // Apply the projection and view transformation
        GLES30.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        checkGlError("glUniformMatrix4fv");

        // Draw the triangle
        GLES30.glDrawArrays(GLES30.GL_LINES, 0, vertexCount);

        // Disable vertex array
        GLES30.glDisableVertexAttribArray(mPositionHandle);
        GLES30.glDisableVertexAttribArray(mPotentialHandle);
    }

    private void updatePosBuffer(){
        float[] connections = getSynConnections();

        vertexCount = connections.length / COORDS_PER_VERTEX;

        ByteBuffer bb = ByteBuffer.allocateDirect(connections.length * 4);
        bb.order(ByteOrder.nativeOrder());

        synPosBuffer = bb.asFloatBuffer();
        synPosBuffer.put(connections);
        synPosBuffer.position(0);
    }

    private void updatePotBuffer(){
        float[] potentials = getSynPotentials();

        ByteBuffer bb = ByteBuffer.allocateDirect(potentials.length * 4);
        bb.order(ByteOrder.nativeOrder());

        synPotBuffer = bb.asFloatBuffer();
        synPotBuffer.put(potentials);
        synPotBuffer.position(0);
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
