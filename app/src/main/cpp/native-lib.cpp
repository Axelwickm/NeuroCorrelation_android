#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>

#include "NeuCor.h"
#include "NeuCor_Renderer.h"

std::unique_ptr<NeuCor> brain;
std::unique_ptr<NeuCor_Renderer> renderer;
renderArrays rA;

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_init
        (JNIEnv * env, jclass cls) {
    brain = std::unique_ptr<NeuCor>(new NeuCor(250));
    renderer = std::unique_ptr<NeuCor_Renderer>(new NeuCor_Renderer(brain.get()));
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_runBrain
        (JNIEnv * env, jclass cls) {
    brain->run();
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_getRenderData
        (JNIEnv * env, jclass cls) {
    rA = renderer->getRenderArrays();

    jfloatArray syn_connections;
    syn_connections = env->NewFloatArray(rA.syn_connections->size()*3);
    env->SetFloatArrayRegion(syn_connections, 0, rA.syn_connections->size()*3, (float*) &rA.syn_connections->at(0));

    //__android_log_print(ANDROID_LOG_INFO,"hejhej","%d", env->GetFloatArrayElements(syn_connections, 0));
}

extern "C"
JNIEXPORT jfloatArray JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_getSynConnections
        (JNIEnv * env, jclass cls) {

    jfloatArray syn_connections;
    syn_connections = (*env).NewFloatArray(rA.syn_connections->size()*3);
    (*env).SetFloatArrayRegion(syn_connections, 0, rA.syn_connections->size()*3, (float*) &rA.syn_connections->at(0));

    delete rA.syn_connections;

    return syn_connections;
}

extern "C"
JNIEXPORT jfloatArray JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_getPotentials
        (JNIEnv * env, jclass cls) {

    jfloatArray syn_potential;
    syn_potential = (*env).NewFloatArray(rA.syn_potential->size());
    (*env).SetFloatArrayRegion(syn_potential, 0, rA.syn_potential->size(), &rA.syn_potential->at(0));

    delete rA.syn_potential;

    return  syn_potential;
}