#pragma once

struct ApplicationState;
struct EngineState;

typedef void* GameStart(EngineState* engine);
typedef void GameRender(EngineState* engine, void* gameState, float dt);
typedef void GameUpdate(EngineState* engine, float dt);
//typedef void* GameReload(void* gameState, Renderer* renderer, const char* filePath);
typedef void GameStop(EngineState* engine, void* gameState);

extern GameStart*  platformGameStart;
extern GameRender* platformGameRender;
extern GameUpdate* platformGameUpdate;
extern GameStop*   platformGameStop;

void platformLoadGame(const char* dllName);
void platformUnloadGame();
bool platformReloadGame(ApplicationState* app, const char* dllName);