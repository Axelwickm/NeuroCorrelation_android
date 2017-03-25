#include "NeuCor_Renderer.h"
#include <android/log.h>

/*  .h & .cpp includes  */
#include "NeuCor.h"

/*  .cpp includes  */

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <stdio.h>

#include "glm/glm.hpp"
using namespace glm;
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLES3/gl3.h>
#include <gtc/type_ptr.inl>


NeuCor_Renderer::NeuCor_Renderer(NeuCor* _brain)
:camPos(5,5,5), camDir(0,0,0), camUp(0,1,0), camHA(0.75), camVA(3.8), lastTime(0), deltaTime(1)
{
    brain = _brain;
    runBrainOnUpdate = true;
    paused = false;
    realRunspeed = false;

    navigationMode = true;
    mouseInWindow = true;
    showInterface = true;

    /*width = ANativeWindow_getWidth(window);
    height = ANativeWindow_getHeight(window);*/
    width = 500;
    height = 500;
    cameraRadius = 8.0f;

    cameraMode = cameraModes::CAMERA_ORBIT_MOMENTUM;
    renderMode = renderingModes::RENDER_VOLTAGE;

    FPS = 0;

    //realTimeStats logger();

    activityExpression = (char*) calloc(256, 1);
    activityExpression[0] = 'a';
    closenessIntensity = 0.8;

    /* Load resources */
    updateCamPos();
    navigationMode = true;
    destructCallback = NULL;
}

NeuCor_Renderer::realTimeStats::realTimeStats(){
    maxTimeline = 15.0; // ms

    activityUpdateTimer = 0;
    weightUpdateTimer = 0.1; // Offset as to not use CPU at the same time
}

NeuCor_Renderer::~NeuCor_Renderer() {
    /* Destroy window and terminate glfw */
    exit(EXIT_SUCCESS);

    /* Call the destruct callback function if it has been set */
    if (destructCallback != NULL) destructCallback();
}

void NeuCor_Renderer::initOpenGL(){
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(GLclampf(0.01), GLclampf(0.01), GLclampf(0.02), GLclampf(1.0));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth(2.5);

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

}

/*bool NeuCor_Renderer::selectNeuron(int id, bool windowOpen){
   /* assert(id < brain->neurons.size());
    if (neuronWindows.find(id) == neuronWindows.end()){
        selectedNeurons.push_back(id);
        logger.timeline.insert(std::pair<int, std::deque<realTimeStats::neuronSnapshot> >(id, std::deque<realTimeStats::neuronSnapshot>()));
        neuronWindow newWindow = {
            .open = windowOpen,
            .usingRelative = true, .relativePosition = ImVec2(15, 15),
            .nextPosition = ImVec2(-1,-1), .nextCollapsed = 0,
            .beingDragged = false
        };

        neuronWindows.insert(std::pair<int, neuronWindow> (id, newWindow));
        if (renderMode == RENDER_CLOSENESS) updateSignalSpread();

        return true;
    }
    else {
        neuronWindows.at(id).open = windowOpen;
        return false;
    }*/
/*}

bool NeuCor_Renderer::deselectNeuron(int id){
    /*assert(id < brain->neurons.size());
    if (neuronWindows.find(id) != neuronWindows.end()){
        for (std::vector<int>::iterator i = selectedNeurons.begin(); i < selectedNeurons.end(); i++){
            if (*i == id){
                selectedNeurons.erase(i);
                break;
            }
        }
        neuronWindows.erase(id);
        logger.timeline.erase(id);
        if (renderMode == RENDER_CLOSENESS) updateSignalSpread();
        return true;
    }
    else {
        return false;
    }*/
//}

void NeuCor_Renderer::updateView(){
    #define PRINT_CONNECTIONS_EVERY_FRAME false

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    double currentTime = now.tv_sec*1000000000LL + now.tv_nsec;

    /*glClearColor((float)rand()/RAND_MAX,(float) rand()/RAND_MAX,(float) rand()/RAND_MAX, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/


    deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    FPS = (FPS*20.0+1.0/deltaTime)/21.0; // Makes FPS change slower
    float aspect = (float) width / (float)height;

    if (runBrainOnUpdate && realRunspeed && !paused) {
        float staticRunSpeed = brain->runSpeed;
        brain->runSpeed = staticRunSpeed * deltaTime;
        brain->run();
        brain->runSpeed = staticRunSpeed;
    }
    else if (runBrainOnUpdate && !paused) brain->run();

    updateCamPos();

    if (renderMode == RENDER_NOSYNAPSES) goto renderNeurons; // Skip rendering synapses

    // Render synapses
    renderSynapses:

    glUseProgram(synapseProgramID);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, synapse_PT_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, connections.size() * sizeof(coord3), NULL);
    glBufferData(GL_ARRAY_BUFFER, connections.size() * sizeof(coord3), &connections[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, synapse_potential_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, synPot.size() * sizeof(GLfloat), NULL);
    glBufferData(GL_ARRAY_BUFFER, synPot.size() * sizeof(GLfloat), &synPot[0], GL_DYNAMIC_DRAW);
    */

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, synapse_PT_buffer);
    glVertexAttribPointer(
     0, // attribute.
     3, // size
     GL_FLOAT, // type
     GL_FALSE, // normalized?
     0, // stride
     (void*)0 // array buffer offset
    );


    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1,0);
    glBindBuffer(GL_ARRAY_BUFFER, synapse_potential_buffer);
    glVertexAttribPointer(
     1, // attribute.
     1, // size
     GL_FLOAT, // type
     GL_FALSE, // normalized?
     0, // stride
     (void*)0 // array buffer offset
    );

    glUniform1f(aspectID[1], aspect);
    glUniformMatrix4fv(ViewProjMatrixID[1], 1, GL_FALSE, &vp[0][0]);

    //glDrawArrays(GL_LINES, 0, connections.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


    // Render neurons
    renderNeurons:

    glUseProgram(neuronProgramID);


    glUniform1f(aspectID[0], aspect);
    glUniformMatrix4fv(ViewProjMatrixID[0], 1, GL_FALSE, &vp[0][0]);


    unsigned neuronC = brain->positions.size();

    glBindBuffer(GL_ARRAY_BUFFER, neuron_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, neuronC * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // Buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, neuronC * 3 * sizeof(GLfloat), &brain->positions[0]);

    glBindBuffer(GL_ARRAY_BUFFER, neuron_potAct_buffer);
    glBufferData(GL_ARRAY_BUFFER, neuronC * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // Buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, neuronC * 2 * sizeof(GLfloat), &brain->potAct[0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
     0, // attribute
     3, // size
     GL_FLOAT, // type
     GL_FALSE, // normalized?
     0, // stride
     (void*)0 // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, neuron_position_buffer);
    glVertexAttribPointer(
     1, // attribute
     3, // size : x + y + z => 3
     GL_FLOAT, // type
     GL_FALSE, // normalized?
     0, // stride
     (void*)0 // array buffer offset
    );

    // 3rd attribute buffer
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, neuron_potAct_buffer);
    glVertexAttribPointer(
     2, // attribute
     2, // values
     GL_FLOAT, // type
     GL_FALSE, // normalized?
     0, // stride
     (void*)0 // array buffer offset
    );


    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, neuronC);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    if (selectedNeurons.size() != 0 && !paused){
        float brainTime = brain->getTime();
        for (auto neuID: selectedNeurons) {
            Neuron* neu = brain->getNeuron(neuID);
            std::deque<realTimeStats::neuronSnapshot>* neuTimeline;
            neuTimeline = &logger.timeline.at(neuID);

            neuTimeline->emplace_back(realTimeStats::neuronSnapshot());
            auto neuSnap = &neuTimeline->back();

            neuSnap->id = neuID;
            neuSnap->time = brainTime;
            neuSnap->voltage = neu->potential();
            neuSnap->synapseWeights.reserve(neu->outSynapses.size());
            for (auto &syn: neu->outSynapses){
                neuSnap->synapseWeights.push_back(syn.getWeight());
            }

            while (logger.maxTimeline < brainTime - neuTimeline->begin()->time)
                neuTimeline->pop_front();
        }
    }
    //if (showInterface) renderInterface();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void NeuCor_Renderer::changeCamera(float dx, float dy){
    deltaX += dx;
    deltaY += dy;
}

std::vector<float> NeuCor_Renderer::getMVPmatrix(float ratio) {
    deltaTime = 0.01;
    static glm::vec2 momentum(0.0, 0.0);

    glm::vec2 cameraPan(0.0, 0.0);
    /*if (ImGui::IsMouseDragging(0, 0) && navigationMode){
        cameraPan = glm::vec2(-deltaX, deltaY)/50.f;
    }*/

    if (cameraMode == CAMERA_ORBIT_MOMENTUM){
        if (navigationMode) momentum = (momentum*15.f+glm::vec2(deltaX, deltaY))/16.f; // deltaTime;
        camHA += momentum.x*deltaTime;
        camVA += momentum.y*deltaTime;
    }
    else {
        camHA -= 0.15 * deltaTime * deltaX;
        camVA  += 0.15 * deltaTime * deltaY;
        momentum = glm::vec2(0.0, 0.0);
    }
    deltaX = 0.0; deltaY = 0.0;

    glm::vec3 focusPoint(0.0, 0.0, 0.0);

    camPos = focusPoint + glm::vec3(cos(camHA) * cos(camVA), sin(camVA), -sin(camHA) * cos(camVA)) * cameraRadius;
    camUp =  glm::normalize(
            glm::vec3(cos(camHA) * cos(camVA + 1.570796f),
                      sin(camVA + 1.570796f),
                      -sin(camHA) * cos(camVA + 1.570796f))
    );
    glm::mat4 Projection = glm::perspective(glm::radians(65.0f), (float) ratio, 0.05f, 100.0f);
    glm::mat4 View = glm::lookAt(
            camPos,
            focusPoint,
            camUp
    );
    //focusPoint += glm::vec3( glm::vec4(cameraPan, 0.0, 0.0) * View );

    vp = Projection * View; // Caluclate matrix

    /*__android_log_print(ANDROID_LOG_VERBOSE, "DDD Vec CamPos C++", "%f", camPos[0]);
    __android_log_print(ANDROID_LOG_VERBOSE, "DDD Vec focusPoint C++", "%f", focusPoint[0]);
    __android_log_print(ANDROID_LOG_VERBOSE, "DDD Vec camUp C++", "%f", camUp[0]);
    __android_log_print(ANDROID_LOG_VERBOSE, "DDD Vec Camera radius C++", "%f", cameraRadius);*/
    // Turn it into vector
    std::vector<float> MVPMatrix;
    MVPMatrix.resize(16, 0);
    const float *pSource = (const float*)glm::value_ptr(vp);
    for (int i = 0; i<16; i++) MVPMatrix[i] = pSource[i];
    return MVPMatrix;
}

renderArrays NeuCor_Renderer::getRenderArrays() {
    if (renderMode == RENDER_NOSYNAPSES) logger.synapseCount = 0;

    closenessValues.resize(brain->neurons.size(), 0);
    std::vector<coord3>* connections = new std::vector<coord3>;
    std::vector<float>* synPot = new std::vector<float>;
    synPot->reserve(brain->neurons.size() * 8.0);
    for (auto &neu : brain->neurons) {
        for (auto &syn : neu.outSynapses) {

            connections->push_back(brain->getNeuron(syn.pN)->position());
            if (connections->back().x != connections->back().x) { // For debugging
                std::cout << "NaN coord!\n";
            }
            connections->push_back(brain->getNeuron(syn.tN)->position());
            if (connections->back().x != connections->back().x) { // For debugging
                std::cout << "NaN coord!\n";
            }
            if (PRINT_CONNECTIONS_EVERY_FRAME)
                std::cout << syn.pN << " " << connections->at(connections->size() - 2).x <<
                " -> " << syn.tN << " " << connections->back().x << " | ";


            if (renderMode == RENDER_VOLTAGE) {
                synPot->push_back(syn.getPrePot() + 0.03);
                synPot->push_back(syn.getPostPot() + 0.03);
            }
            else if (renderMode == RENDER_PLASTICITY) {
                synPot->push_back(syn.getWeight() / 2.0);
                synPot->push_back(syn.getWeight() / 2.0);
                if (RENDER_PLASTICITY_onlyActive) {
                    synPot->at(synPot->size() - 2) *= log(
                            brain->getNeuron(syn.pN)->activity() + 1.f);
                    synPot->back() *= log(brain->getNeuron(syn.tN)->activity() + 1.f);
                }

            }
            else if (renderMode == RENDER_ACTIVITY) {
                synPot->push_back(log(brain->getNeuron(syn.pN)->activity() + 1.f));
                synPot->push_back(log(brain->getNeuron(syn.tN)->activity() + 1.f));
            }
            else if (renderMode == RENDER_CLOSENESS) {
                synPot->push_back(powf(closenessValues.at(syn.pN), closenessIntensity));
                synPot->push_back(powf(closenessValues.at(syn.tN), closenessIntensity));
            }
            else if (renderMode == RENDER_NOSYNAPSES) logger.synapseCount++;
        }
    }
    if (PRINT_CONNECTIONS_EVERY_FRAME) std::cout << std::endl;
    logger.neuronCount = brain->neurons.size();
    if (renderMode != RENDER_NOSYNAPSES) logger.synapseCount = synPot->size() / 2;

    renderArrays rA;
    rA.syn_connections = connections;
    rA.syn_potential = synPot;
    return rA;
}

void NeuCor_Renderer::setDestructCallback(CallbackType callbackF){
    destructCallback = callbackF;
}

void NeuCor_Renderer::updateCamPos(){
    if ((!navigationMode && !(cameraMode == CAMERA_ORBIT_MOMENTUM)) || !mouseInWindow) return;

    float aspect = (float) width / (float)height;
    double xpos, ypos;
    //glfwGetCursorPos(window, &xpos, &ypos);
    xpos = width/2.f;
    ypos = height/2.f;

    if (cursorX != cursorX) {cursorX = xpos; cursorY = ypos;}

    if (cameraMode == CAMERA_MOUSE_LOOK){
        float deltaX = cursorX-xpos;
        float deltaY = cursorY-ypos;
        deltaX = 0; deltaY = 0; deltaTime = 0;
        camHA += 0.1;
        camVA += 0.5;

        camHA -= 0.15 * deltaTime * deltaX;
        camVA  += 0.15 * deltaTime * deltaY;

        camDir = glm::vec3 (
            cos(camVA) * sin(camHA),
            sin(camVA),
            cos(camVA) * cos(camHA)
        );
        glm::vec3 right = glm::vec3(
            -cos(camHA),
            0,
            sin(camHA)
        );
        camUp = glm::cross( right, camDir );

        float speedMult = 5;
        // Move forward
        /*if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS){
            camPos += camDir * GLfloat(deltaTime * speedMult);
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS){
            camPos -= camDir * GLfloat(deltaTime * speedMult);
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS){
            camPos += right * GLfloat(deltaTime *  speedMult);
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
            camPos -= right * GLfloat(deltaTime * speedMult);
        }*/

        glm::mat4 Projection = glm::perspective(glm::radians(65.0f), (float) aspect, 0.05f, 100.0f);
        glm::mat4 View = glm::lookAt(
            camPos,
            camPos+camDir,
            camUp
        );
        vp = Projection * View;

        //cameraRadius = glm::distance(camPos, vec3(0,0,0)); // For orbit camera mode
    }
    else if (cameraMode == CAMERA_ORBIT || cameraMode == CAMERA_ORBIT_MOMENTUM) {
        float deltaX = cursorX-xpos;
        float deltaY = cursorY-ypos;
        static glm::vec2 momentum(0.0, 0.0);

        glm::vec2 cameraPan(0.0, 0.0);
        /*if (ImGui::IsMouseDragging(0, 0) && navigationMode){
            cameraPan = glm::vec2(-deltaX, deltaY)/50.f;
        }*/
        if (false){}
        else {
            if (cameraMode == CAMERA_ORBIT_MOMENTUM){
                if (navigationMode) momentum += glm::vec2(deltaX, -deltaY) * deltaTime / 100.f;
                camHA += momentum.x*deltaTime;
                camVA += momentum.y*deltaTime;
            }
            else {
                camHA -= 0.15 * deltaTime * deltaX;
                camVA  += 0.15 * deltaTime * deltaY;
                momentum = glm::vec2(0.0, 0.0);
            }
        }
        static glm::vec3 focusPoint(0.0, 0.0, 0.0);

        camPos = focusPoint + glm::vec3(cos(camHA) * cos(camVA), sin(camVA), -sin(camHA) * cos(camVA)) * cameraRadius;
        camUp =  glm::normalize(
            glm::vec3(cos(camHA) * cos(camVA + 1.570796f),
            sin(camVA + 1.570796f),
            -sin(camHA) * cos(camVA + 1.570796f))
        );
        glm::mat4 Projection = glm::perspective(glm::radians(65.0f), (float) aspect, 0.05f, 100.0f);
        glm::mat4 View = glm::lookAt(
            camPos,
            focusPoint,
            camUp
        );
        focusPoint += glm::vec3( glm::vec4(cameraPan, 0.0, 0.0) * View );

        vp = Projection * View;
    }
    cursorX = xpos; cursorY = ypos;
};

inline glm::vec3 NeuCor_Renderer::screenCoordinates(glm::vec3 worldPos, bool nomalizedZ){
    glm::vec4 posClip = vp * glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f );
    glm::vec3 posNDC = glm::vec3(posClip) / posClip.w;
    if (1.0 < posNDC.z) return glm::vec3(NAN, NAN, NAN); // Don't give coordinates if behind camera
    if (nomalizedZ){
        GLfloat dR[2]; // Depth range
        glGetFloatv(GL_DEPTH_RANGE, &dR[0]);
        return glm::vec3(
            posNDC.x*width*0.5+width*0.5,
            -posNDC.y*height*0.5+height*0.5,
            (dR[1]-dR[0])/2.0*posNDC.z + (dR[1]+dR[0])/2.0);
    }
    else {
        return glm::vec3(
            posNDC.x*width*0.5+width*0.5,
            -posNDC.y*height*0.5+height*0.5,
            posNDC.z);
    }
}

float NeuCor_Renderer::getDeltaTime(){
    return deltaTime;
}

template<typename ... callbackParameters>
void NeuCor_Renderer::inputCallback(callbackErrand errand, callbackParameters ... params){
    std::tuple<callbackParameters...> TTparams(params... );

    switch (errand){

    case (KEY_ACTION):
        break;

    case (CHAR_ACTION):
        if (std::get<1>(TTparams) == 99 || std::get<1>(TTparams) == 67){ // Iterate to next camera mode on C-key press
            cameraMode = static_cast<cameraModes>(cameraMode+1);
            if (cameraMode == cameraModes::CAMERA_count) cameraMode = static_cast<cameraModes>(cameraMode-(int) cameraModes::CAMERA_count);
            std::cout<<"Camera mode: "<<cameraModeNames.at(cameraMode)<<std::endl;
        }
        else if (std::get<1>(TTparams) == 109 || std::get<1>(TTparams) == 77){ // Iterate to next rendering mode on M-key press
            renderMode = static_cast<renderingModes>(renderMode+1);
            if (renderMode == renderingModes::RENDER_count) renderMode = static_cast<renderingModes>(renderMode-(int) renderingModes::RENDER_count);
            std::cout<<"Rendering mode: "<<renderingModeNames.at(renderMode)<<std::endl;
        }
        break;

    case (MOUSE_BUTTON):
        break;

    case (MOUSE_SCROLL):
        if (cameraMode == CAMERA_ORBIT || cameraMode == CAMERA_ORBIT_MOMENTUM){
            cameraRadius -= std::get<2>(TTparams)/4.0f;
        }
        break;

    case (MOUSE_ENTER):
        break;
    }
}
