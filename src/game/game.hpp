#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "core.hpp"
#include "scene.hpp"

//struct AnimationManager{
//    std::unordered_map<std::string, AnimationComponent> animations;
//};

struct GameState{
    //Scene scene;
    AnimationManager animationManager;
    Ecs* ecs;
    OrtographicCamera camera;
    Entity player;
    TileMap bgMap;
    TileMap fgMap;
};

extern "C" {
    GAME_API GameState* gameStart(Renderer* renderer);
    GAME_API void gameRender(GameState* gameState, Renderer* renderer, float dt);
    GAME_API void gameUpdate(GameState* gameState, Input* input, float dt);
    GAME_API void gameStop();
}