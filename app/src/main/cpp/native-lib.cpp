#include <jni.h>
#include <android/native_window_jni.h>
#include <string>

#include "NeuCor.h"
#include "NeuCor_Renderer.h"
#include <memory>

std::unique_ptr<NeuCor> brain;
std::unique_ptr<NeuCor_Renderer> renderer;

extern "C"
void Java_com_example_axel_spikingbrain_MainActivity_initBrain(
        JNIEnv* env,
        jobject /* this */) {
    brain = std::unique_ptr<NeuCor>(new NeuCor(250));
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_on_1surface_1created
        (JNIEnv * env, jclass cls) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, cls);
    renderer = std::unique_ptr<NeuCor_Renderer>(new NeuCor_Renderer(brain.get(), window));
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_on_1surface_1changed
        (JNIEnv * env, jclass cls, jint width, jint height) {
    //on_surface_changed();
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_axel_spikingbrain_LibJNIWrapper_on_1draw_1frame
        (JNIEnv * env, jclass cls) {
    renderer->updateView();
}