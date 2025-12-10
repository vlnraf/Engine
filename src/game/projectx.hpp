#pragma once

#if defined(_WIN32)
    #ifdef GAME_EXPORT
        #define GAME_API __declspec(dllexport)
    #else
        #define GAME_API __declspec(dllimport)
    #endif
#else
    // Linux / macOS
    #ifdef GAME_EXPORT
        #define GAME_API __attribute__((visibility("default")))
    #else
        #define GAME_API
    #endif
#endif

#include "core.hpp"
//#include "gamekit/animationmanager.hpp"
#include "mainmenu.hpp"

#define CARDS_NUMBER 6

enum CardChoice{
    CARD_DMG_UP,
    CARD_SPEED_UP,
    CARD_SIZE_UP,
    CARD_ADD_PROJECTILE,
    CARD_ORBIT,
    CARD_GRANADE,
    CARD_NONE
};

struct Card{
    char description[500];
    float dmg = 0;
    float speed = 0;
    float radius = 0;
    bool pickable = true;
    uint32_t level;
    uint32_t maxLevel;

    CardChoice cardChoice;
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
    Arena* arena;
    OrtographicCamera mainCamera;
    TileMap bgMap;
    TileMap fgMap;
    RenderTexture renderTexture;
    Texture* backGround;
    Font* defaultFont;
    bool restart;

    GameLevels gameLevels;
    Card cards[6];
    // = {
    //    {.description = "increase \ndamage \nof 20%", .dmg = 0.2f, .speed = 0, .cardChoice = CardChoice::CARD_DMG_UP},
    //    {.description = "increase \nspeed \nof 20%", .dmg = 0.0f, .speed = 0.2f, .cardChoice = CardChoice::CARD_SPEED_UP},
    //    {.description = "increase \nprojectile \nof 20%", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f, .cardChoice = CardChoice::CARD_SIZE_UP},
    //    {.description = "+1 projectiles", .cardChoice = CardChoice::CARD_ADD_PROJECTILE},
    //    {.description = "Add \nOrbit Weapon", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f, .cardChoice = CardChoice::CARD_ORBIT},
    //    {.description = "launch a\ngranade each\nsecond", .cardChoice = CardChoice::CARD_GRANADE}
    //};
    //Card runCards[5];
    float cardInit = false;

    MenuState menuState = {};

    bool pause = false;
    Shader shader;
    float shaderTime = 0.0f;  // Accumulated time for shader effects
};

extern GameState* gameState;
extern EngineState* engine;


extern "C" {
    GAME_API void gameStart(Arena* gameArena, EngineState* engine);
    GAME_API void gameRender(Arena* gameArena, EngineState* engine, float dt);
    GAME_API void gameUpdate(Arena* gameArena, EngineState* engine, float dt);
    //GAME_API GameState* gameReload(GameState* gameState, Renderer* renderer, const char* filePath);
    GAME_API void gameStop(Arena* gameArena, EngineState* engine);
}

void loadLevel(GameLevels level);