#pragma once

#include "core/coreapi.hpp"

struct ApplicationState;
struct EngineState;
struct Arena;

typedef void GameStart(Arena* gameArena, EngineState* engine);
typedef void GameRender(Arena* gameArena, EngineState* engine, float dt);
typedef void GameUpdate(Arena* gameArena, EngineState* engine, float dt);
//typedef void* GameReload(void* gameState, Renderer* renderer, const char* filePath);
typedef void GameStop(Arena* gameArena, EngineState* engine);

extern GameStart*  platformGameStart;
extern GameRender* platformGameRender;
extern GameUpdate* platformGameUpdate;
extern GameStop*   platformGameStop;

void platformLoadGame(const char* dllName);
void platformUnloadGame();
bool platformReloadGame(const char* dllName);

void memSet(void* dst, int value, size_t size);
void memCopy(void* dst, const void* src, size_t size);