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


//void initWindow(ApplicationState* app, const char* name, const uint32_t width, const uint32_t height);
CORE_API void updateAndRender(ApplicationState* app);
CORE_API bool applicationShouldClose(ApplicationState* app);
CORE_API ApplicationState initApplication(int width, int height);
CORE_API void applicationRun(ApplicationState* app);
CORE_API void applicationShutDown(ApplicationState* app);

// Callback function pointer for quit request
typedef void (*QuitCallback)();

// Set the quit callback (called by application layer during init)
CORE_API void applicationSetQuitCallback(QuitCallback callback);

// Request application to quit (callable from game layer)
CORE_API void applicationRequestQuit();

CORE_API void logger();