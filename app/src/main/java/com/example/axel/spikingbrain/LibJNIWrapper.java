package com.example.axel.spikingbrain;

public class LibJNIWrapper {
    static {
        System.loadLibrary("native-lib"); // Laddar in från C++
    }

    // Skapar hjärna, och renderare
    public static native void init();

    // Simulerar hjärna
    public static native void runBrain();

    // Ger MVP-matris
    public static native float[] getMVPMatrix(float MVPmatrix);

    // Hämtar renderingsdata från NeuCor_Renderer, och sparar det den i medlemsvariabler
    public static native void getRenderData();

    // Ger koorinater till hörnen (synapsändarna)
    public static native float[] getSynConnections();

    // Ger potentialer till hörnen
    public static native float[] getSynPotentials();

    // Tar bort renderingsdata (måste anropas efer getRenderData för att inte skapa minnesläckor)
    public static native void clearRenderData();
}