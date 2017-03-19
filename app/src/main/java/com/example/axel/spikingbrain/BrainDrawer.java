package com.example.axel.spikingbrain;

import android.content.Context;
import android.opengl.GLES30;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import static com.example.axel.spikingbrain.LibJNIWrapper.clearRenderData;
import static com.example.axel.spikingbrain.LibJNIWrapper.getRenderData;
import static com.example.axel.spikingbrain.LibJNIWrapper.getSynConnections;
import static com.example.axel.spikingbrain.LibJNIWrapper.getSynPotentials;
import static com.example.axel.spikingbrain.MyGLRenderer.checkGlError;

// Pratar med LibJNIWrapper för att visa en rendering av hjärnan
public class BrainDrawer {
    // Antal koordinater per hörn (synaps-ändar)
    static final int COORDS_PER_VERTEX = 3;
    // Handles till shader-programmet,  GLSL-attributerna (koordinater och potentialer), samt en uniform till MVP-matrisen
    private final int mSynapseProgram;
    private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per hörn
    // Context behövs för att använda AssetManager
    private Context context;
    // FloatBuffrar som kommer ihåg synapsernas start och stop koordinater
    private FloatBuffer synPosBuffer;
    // ... och deras elektriska potentialer
    private FloatBuffer synPotBuffer;
    private int mPositionHandle;
    private int mPotentialHandle;
    private int mMVPMatrixHandle;
    // Antal hörn
    private int vertexCount;

    public BrainDrawer(Context context) {
        // Sparar context
        this.context = context;

        getRenderData();
        updatePosBuffer(); // Hämtar positions-array endast en gång, då neuronernas position ej förändras
        clearRenderData();

        // Laddar in shader-kod från filer i Assets
        String vertexShaderCode = readFileAsString("synapse.shader");
        String fragmentShaderCode = readFileAsString("synapse.Fshader");

        // Skapar shaders
        int vertexShader = MyGLRenderer.loadShader(GLES30.GL_VERTEX_SHADER, vertexShaderCode);
        checkGlError("Load synapse vertex shader"); // Loggar eventuella fel
        int fragmentShader = MyGLRenderer.loadShader(GLES30.GL_FRAGMENT_SHADER, fragmentShaderCode);
        checkGlError("Load synapse fragment shader");

        mSynapseProgram = GLES30.glCreateProgram();             // Skapar tomt OpenGL Program
        checkGlError("Create program");
        GLES30.glAttachShader(mSynapseProgram, vertexShader);   // Lägg till vertex shader
        checkGlError("Attach synapse vertex shader");
        GLES30.glAttachShader(mSynapseProgram, fragmentShader); // Lägg till fragment shader
        checkGlError("Attach synapse fragment shader");
        GLES30.glLinkProgram(mSynapseProgram);                  // Länkar programmet
        checkGlError("Link synapse Program");              // create OpenGL program executables

    }

    public void draw(float[] mvpMatrix) {
        getRenderData();
        updatePotBuffer(); // Hämtar synapsernas spännings-array
        clearRenderData();

        // Ändra linjetjockleken
        GLES30.glLineWidth(2.5f);
        // Använder detta program
        GLES30.glUseProgram(mSynapseProgram);

        // Hämtar handle till position
        mPositionHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPosition");

        // Sätter på handle
        GLES30.glEnableVertexAttribArray(mPositionHandle);

        // Laddar över data (koordinater)
        GLES30.glVertexAttribPointer(
                mPositionHandle, COORDS_PER_VERTEX,
                GLES30.GL_FLOAT, false,
                vertexStride, synPosBuffer);


        // Gör samma för potential
        mPotentialHandle = GLES30.glGetAttribLocation(mSynapseProgram, "vPotential");
        GLES30.glEnableVertexAttribArray(mPotentialHandle);
        GLES30.glVertexAttribPointer(
                mPotentialHandle, 1,
                GLES30.GL_FLOAT, false,
                4, synPotBuffer);

        // Hämtar handle till MVP-matris
        mMVPMatrixHandle = GLES30.glGetUniformLocation(mSynapseProgram, "uMVPMatrix");
        checkGlError("glGetUniformLocation");

        // Laddar över matrisen
        GLES30.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        checkGlError("glUniformMatrix4fv");

        // Ritar hjärnan
        GLES30.glDrawArrays(GLES30.GL_LINES, 0, vertexCount);

        // Stänger av handles
        GLES30.glDisableVertexAttribArray(mPositionHandle);
        GLES30.glDisableVertexAttribArray(mPotentialHandle);
    }

    // Hämtar koordinater från LibJNIWrapper, och lägger dom i FloatBuffer
    private void updatePosBuffer(){
        // Lägger koordinater i float array
        float[] connections = getSynConnections();

        // Räknar ut nytt antal hörn
        vertexCount = connections.length / COORDS_PER_VERTEX;

        // Lägger data i ByteBuffer
        ByteBuffer bb = ByteBuffer.allocateDirect(connections.length * 4);
        bb.order(ByteOrder.nativeOrder()); // Använder föredragen ordning

        // Gör om till FloatBuffer
        synPosBuffer = bb.asFloatBuffer();
        synPosBuffer.put(connections);
        synPosBuffer.position(0);
    }

    // Gör samma för potentialer
    private void updatePotBuffer(){
        float[] potentials = getSynPotentials();

        ByteBuffer bb = ByteBuffer.allocateDirect(potentials.length * 4);
        bb.order(ByteOrder.nativeOrder());

        synPotBuffer = bb.asFloatBuffer();
        synPotBuffer.put(potentials);
        synPotBuffer.position(0);
    }

    // Hämtar fil, och returnerar string
    private String readFileAsString(String filePath) {
        InputStream input;
        String text = ""; // Ut-String

        try {
            input = context.getAssets().open(filePath); // Öppnar fil

            int size = input.available(); // Räknar ut längd
            // Gör resten
            byte[] buffer = new byte[size];
            input.read(buffer);
            input.close();

            // byte buffer into a string
            text = new String(buffer);

        } catch (IOException e) {// Om det går fel
            e.printStackTrace(); // Logga det
        }

        return text;
    }

}
