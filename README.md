# EXIS

Exis is a simple game engine made to have fun and learn new things during my spare time.
The idea is to have an engine which doesn't give all the graphical tools that are used right now in the industry (atleast right now), and the reason
of this choice is simple, i like programming and so i wan't the engine be based on pure code writing.

## Features
- the engine is modular, you can abstract the core library and use it for other pourpuse
- Very limited set of dependencies
- Based on an Entity Component System (developed from scratch)
- Hardware accelerated with OpenGL
- Resource loading
- Integrated with Tiled (da aggiungere link) to generate awesome maps to import

## Showcase
To use the engine as it is being thinked you need to put your game logic files in the "game" folder under "src" and create atleast one file which communicate with the application in this way

```cpp
#include "projectx.hpp"
GAME_API void gameStart(EngineState* engine){
    GameState* gameState = new GameState();
    engine->gameState = gameState;
    gameState->camera = createCamera({0,0,0}, 640, 320);
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    beginScene(gameState->camera, RenderMode::NORMAL);
    renderDrawFilledRect({0,0}, {200,200}, {0,0}, {1, 0.5, 1, 0});
    endScene();
}
```
and in the hpp file

```cpp
#pragma once

#include "core.hpp"
struct GameState{

};

extern "C" {
    GAME_API void gameStart(EngineState* engine);
    GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt);
}
```

In order to test the features of the engine I am also developing a really simple game

https://github.com/user-attachments/assets/771f2822-f2e4-4b3c-87e0-80f624046a04


## Build
