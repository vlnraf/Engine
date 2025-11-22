#pragma once

#include "platform/platform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
