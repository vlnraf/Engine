#pragma once

#include "windows.h"
#include "Input.hpp"
#include "renderer/renderer.hpp"

struct ApplicationState{
    GLFWwindow* window;
    Input input;
    Renderer renderer;

    int width;
    int height;
};

typedef void* GameStart(const char* testo);
typedef void GameRender(void* gameState, Renderer* renderer);
typedef void GameUpdate(void* gameState, Input* input);

struct Win32DLL{
    HMODULE gameCodeDLL;
    GameStart* gameStart;
    GameRender* gameRender;
    GameUpdate* gameUpdate;

    FILETIME lastWriteTimeOld;
    bool isValid;
};