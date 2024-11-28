#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "Input.hpp"
#include "shader.hpp"
#include "scene.hpp"

struct GameState{
    glm::mat4 transform;
    glm::mat4 translate;
};

extern "C" {
    GAME_API Scene* gameStart(const char* testo);
    GAME_API void gameRender(GameState* gameState);
    GAME_API void gameUpdate(GameState* gameState, Input* input);
}