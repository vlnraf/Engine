#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>

#include "core.hpp"

struct ApplicationState{
    GLFWwindow* window;
    //Input input;
    //Renderer renderer;

    EngineState* engine;

    float lastFrame;
    float startFrame;
    float endFrame;
    float dt;
    float fps;

    int width;
    int height;

    bool reload = false;
};

typedef void GameStart(EngineState* engine);
typedef void GameRender(EngineState* engine, void* gameState, float dt);
typedef void GameUpdate(EngineState* engine, void* gameState, float dt);
//typedef void* GameReload(void* gameState, Renderer* renderer, const char* filePath);
typedef void GameStop(EngineState* engine, void* gameState);

struct Win32DLL{
    HMODULE gameCodeDLL;
    GameStart* gameStart;
    GameRender* gameRender;
    GameUpdate* gameUpdate;
    //GameReload* gameReload;
    GameStop* gameStop;

    FILETIME lastWriteTimeOld;
    bool isValid;
};