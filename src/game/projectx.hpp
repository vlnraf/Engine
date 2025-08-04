#pragma once

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
    #ifdef GAME_EXPORT
        #define GAME_API __declspec(dllexport)
    #else
        #define GAME_API __declspec(dllimport)
    #endif
#else
    #include <emscripten.h>
    #define GAME_API EMSCRIPTEN_KEEPALIVE
#endif

#include "core.hpp"
//#include "gamekit/animationmanager.hpp"
#include "mainmenu.hpp"

struct Card{
    char description[500];
    float dmg = 0;
    float speed = 0;
    float radius = 0;
};


enum GameLevels{
    MAIN_MENU,
    FIRST_LEVEL,
    SECOND_LEVEL,
    THIRD_LEVEL,
    SELECT_CARD,
    GAME_OVER
};


struct GameState{
    //Scene scene;
    //AnimationManager animationManager;
    //Ecs* ecs;
    OrtographicCamera camera;
    TileMap bgMap;
    TileMap fgMap;

    GameLevels gameLevels;
    Card cards[3] = {
        {.description = "increase \ndamage \nof 20%", .dmg = 0.2f, .speed = 0},
        {.description = "increase \nspeed \nof 20%", .dmg = 0.0f, .speed = 0.2f},
        {.description = "increase \nprojectile \nof 20%", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f}
    };

    MenuState menuState = {};

    bool debugMode = false;
    bool pause = false;
};

extern GameState* gameState;


extern "C" {
    GAME_API void* gameStart(EngineState* engine);
    GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt);
    GAME_API void gameUpdate(EngineState* engine, float dt);
    //GAME_API GameState* gameReload(GameState* gameState, Renderer* renderer, const char* filePath);
    GAME_API void gameStop(EngineState* engine, GameState* gameState);
}

void loadLevel(GameLevels level);