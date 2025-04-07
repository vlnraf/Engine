#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "core.hpp"
//#include "gamekit/animationmanager.hpp"

enum GameLevels{
    MAIN_MENU,
    FIRST_LEVEL,
    SECOND_LEVEL,
    GAME_OVER,
    END
};

struct GameState{
    //Scene scene;
    //AnimationManager animationManager;
    //Ecs* ecs;
    OrtographicCamera camera;
    TileMap bgMap;
    TileMap fgMap;

    GameLevels gameLevels;

    bool debugMode = false;
};


extern "C" {
    GAME_API void gameStart(EngineState* engine);
    GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt);
    GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt);
    //GAME_API GameState* gameReload(GameState* gameState, Renderer* renderer, const char* filePath);
    GAME_API void gameStop(EngineState* engine, GameState* gameState);
}

void loadLevel(GameState* gameState, EngineState* engine);
