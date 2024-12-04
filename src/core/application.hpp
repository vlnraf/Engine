#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>

#include "core.hpp"

struct ApplicationState{
    GLFWwindow* window;
    Input* input;
    Renderer* renderer;

    float lastFrame;
    float startFrame;
    float endFrame;
    float dt;
    float fps;

    int width;
    int height;
};

typedef void* GameStart(Renderer* renderer);
typedef void GameRender(void* gameState, Renderer* renderer);
typedef void GameUpdate(void* gameState, Input* input, float dt);

struct Win32DLL{
    HMODULE gameCodeDLL;
    GameStart* gameStart;
    GameRender* gameRender;
    GameUpdate* gameUpdate;

    FILETIME lastWriteTimeOld;
    bool isValid;
};