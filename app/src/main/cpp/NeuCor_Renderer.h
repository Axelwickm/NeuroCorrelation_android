#ifndef NEUCOR_RENDERER_H
#define NEUCOR_RENDERER_H

/*  .h & .cpp includes  */
#include "NeuCor.h"

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <GLES3/gl3.h>
#include <android/native_window_jni.h>

#include "glm/glm.hpp"
using namespace glm;

class NeuCor_Renderer {
    public:
        NeuCor_Renderer(NeuCor* _brain, ANativeWindow* win);
        ~NeuCor_Renderer();

        float getDeltaTime();
        bool runBrainOnUpdate; // If the render has the responsibility to run the brain.
        bool realRunspeed;// Makes brain's runSpeed define simulation's speed by ms/s
        bool paused;
        bool selectNeuron(int id, bool windowOpen);
        bool deselectNeuron(int id);

        enum cameraModes{ CAMERA_MOUSE_LOOK, CAMERA_ORBIT, CAMERA_ORBIT_MOMENTUM, CAMERA_count};
        std::vector<std::string> cameraModeNames = {"Mouse look", "Orbit", "Orbit momentum"};
        cameraModes cameraMode;

        enum renderingModes { RENDER_VOLTAGE, RENDER_PLASTICITY, RENDER_ACTIVITY, RENDER_CLOSENESS, RENDER_NOSYNAPSES, RENDER_count};
        bool RENDER_PLASTICITY_onlyActive = true;
        std::vector<std::string> renderingModeNames = {"Voltage", "Plasticity", "Activity", "Closeness", "No synapses"};
        renderingModes renderMode;
        char* activityExpression;

        void updateView();
        void pollWindow();
        ANativeWindow* window;

        typedef void (*CallbackType)();
        void setDestructCallback(CallbackType f);

        enum callbackErrand {KEY_ACTION, CHAR_ACTION, MOUSE_BUTTON, MOUSE_SCROLL, MOUSE_ENTER };
        template<typename ... callbackParameters>
        void inputCallback(callbackErrand errand, callbackParameters ... params);
    protected:
    private:
        NeuCor* brain;

        std::vector<int> selectedNeurons; // Selected neuron ID, smart pointer to bool if neuron window is open
        std::vector<float> closenessValues; float closenessIntensity;
        void updateSignalSpread();

        /*void renderInterface();
        void renderModule(module* mod, bool windowed);
        void renderNeuronWindow(int ID, neuronWindow* neuWin = NULL);
        inline void renderLine(int ID);*/
        void updateCamPos();
        float cameraRadius;
        void resetCursor();


        struct realTimeStats {
            realTimeStats();
            float activityUpdateTimer, weightUpdateTimer;

            unsigned neuronCount, synapseCount;

            struct neuronSnapshot { // For plotting
                int id;
                float time;
                float voltage;
                std::vector<float> synapseWeights;
            };
            std::map<int, std::deque<neuronSnapshot> > timeline;
            float maxTimeline;
        };
        realTimeStats logger;

        GLuint billboard_vertex_buffer;
        GLuint neuron_position_buffer;
        GLuint neuron_potAct_buffer;
        GLuint synapse_PT_buffer;
        GLuint synapse_potential_buffer;

        GLuint neuronTexID;
        GLuint neuron_smallTexID;
        GLuint neuronProgramID;
        GLuint synapseProgramID;
        GLuint ViewProjMatrixID[2];
        GLuint aspectID[2];

        glm::vec3 camPos;
        glm::vec3 camDir;
        glm::vec3 camUp;
        float camHA,camVA;
        glm::mat4 vp;
        double cursorX, cursorY;
        bool navigationMode, mouseInWindow, showInterface;

        CallbackType destructCallback;

        int width, height;
        double lastTime;
        float deltaTime;
        float FPS;
        void initOpenGL();
        void loadResources();
        inline glm::vec3 screenCoordinates(glm::vec3 worldPos, bool nomalizedZ = false);
};

#endif // NEUCOR_RENDERER_H
