#pragma once

#include <cstddef>
#include "core/coreapi.hpp"

struct ApplicationState;
struct EngineState;
struct Arena;

typedef void GameStart(Arena* gameArena, EngineState* engine);
typedef void GameRender(Arena* gameArena, EngineState* engine, float dt);
typedef void GameUpdate(Arena* gameArena, EngineState* engine, float dt);
//typedef void* GameReload(void* gameState, Renderer* renderer, const char* filePath);
typedef void GameStop(Arena* gameArena, EngineState* engine);

extern CORE_API GameStart*  platformGameStart;
extern CORE_API GameRender* platformGameRender;
extern CORE_API GameUpdate* platformGameUpdate;
extern CORE_API GameStop*   platformGameStop;

CORE_API void platformLoadGame(const char* dllName);
CORE_API void platformUnloadGame();
CORE_API bool platformReloadGame(const char* dllName);

CORE_API void memSet(void* dst, int value, size_t size);
CORE_API void memCopy(void* dst, const void* src, size_t size);