#pragma once

#include "window.hpp"
#include "platform/platform.hpp"


struct ApplicationState{
    //GLFWwindow* window;
    Window window;

    EngineState* engine;

    float lastFrame;
    float startFrame;
    float endFrame;
    float dt;
    float fps;

    //int width;
    //int height;

    bool debugMode = false;
    bool reload = false;
    bool quit = false;
};


// ============================================================================
// INTERNAL APPLICATION API - NOT FOR GAME USE
// These functions are exported for application.exe but should not be called from game code
// ============================================================================

// Application lifecycle functions (internal - called by application.exe main loop)
CORE_API void updateAndRender();
CORE_API bool applicationShouldClose();
CORE_API ApplicationState initApplication(const char* name, int width, int height);
CORE_API void applicationRun();
CORE_API void applicationShutDown();

// ============================================================================
// PUBLIC GAME API - Safe for game code to call
// ============================================================================

// Request application to quit from game code
CORE_API void applicationRequestQuit();
CORE_API void applicationSetResolution(int width, int height);