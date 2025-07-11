#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "core.hpp"
#include "scene.hpp"
#include "gamekit/animationmanager.hpp"
#include "gamekit/colliders.hpp"

#include "hitbox.hpp"

struct GameState{
    //Scene scene;
    AnimationManager animationManager;
    Ecs* ecs;
    OrtographicCamera camera;
    TileMap bgMap;
    TileMap fgMap;
    

    bool debugMode = false;
};


extern "C" {
    GAME_API void gameStart(EngineState* engine);
    GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt);
    GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt);
    //GAME_API GameState* gameReload(GameState* gameState, Renderer* renderer, const char* filePath);
    GAME_API void gameStop(EngineState* engine, GameState* gameState);
}
