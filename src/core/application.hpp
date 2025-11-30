#pragma once

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include "platform/platform.hpp"
#include "core.hpp"


struct ApplicationState{
    GLFWwindow* window;

    EngineState* engine;

    float lastFrame;
    float startFrame;
    float endFrame;
    float dt;
    float fps;

    int width;
    int height;

    bool debugMode = false;
    bool reload = false;
};


void initWindow(ApplicationState* app, const char* name, const uint32_t width, const uint32_t height);
void updateAndRender(ApplicationState* app);
bool applicationShouldClose(ApplicationState* app);
ApplicationState initApplication(int width, int height);
void applicationRun(ApplicationState* app);
void applicationShutDown(ApplicationState* app);

CORE_API void logger();