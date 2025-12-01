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
CORE_API void updateAndRender(ApplicationState* app);
CORE_API bool applicationShouldClose(ApplicationState* app);
CORE_API ApplicationState initApplication(int width, int height);
CORE_API void applicationRun(ApplicationState* app);
CORE_API void applicationShutDown(ApplicationState* app);

// Callback function pointer for quit request (internal)
typedef void (*QuitCallback)();

// Set the quit callback (internal - called once by application.exe during init)
CORE_API void applicationSetQuitCallback(QuitCallback callback);

// ============================================================================
// PUBLIC GAME API - Safe for game code to call
// ============================================================================

// Request application to quit from game code
CORE_API void applicationRequestQuit();