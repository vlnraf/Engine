#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "Input.hpp"
#include "shader.hpp"
#include "renderer/renderer.hpp"

struct Model{
    VertexBuffer vertices;
};
struct Object{
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec2 texCoord;
    glm::vec3 color;

    Model model;
};

struct GameState{
    Shader shader;

    glm::mat4 transform;
    glm::mat4 translate;

    Object object[2];
};

extern "C" {
    GAME_API void* gameStart(const char* testo);
    GAME_API void gameRender(GameState* gameState, Renderer* renderer);
    GAME_API void gameUpdate(GameState* gameState, Input* input);
}