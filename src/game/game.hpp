#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "Input.hpp"
#include "shader.hpp"
#include "scene.hpp"

extern "C" {
    GAME_API Scene* gameStart(const char* testo, Renderer* renderer);
    GAME_API void gameRender(Scene* gameState, Renderer* renderer);
    GAME_API void gameUpdate(Scene* gameState, Input* input);
}