#pragma once

#include "windows.h"
#include "input.hpp"
#include "renderer/renderer.hpp"
#include "scene.hpp"
#include "ecs.hpp"

struct ApplicationState{
    GLFWwindow* window;
    Input* input;
    Renderer* renderer;

    int width;
    int height;
};

typedef void* GameStart(const char* testo, Renderer* renderer);
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