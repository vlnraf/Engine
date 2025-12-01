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

// ============================================================================
// INTERNAL PLATFORM API - NOT FOR GAME USE
// These are exported for core/application use but should not be called from game code
// ============================================================================

// Game function pointers (internal - managed by platform layer)
extern GameStart*  platformGameStart;
extern GameRender* platformGameRender;
extern GameUpdate* platformGameUpdate;
extern GameStop*   platformGameStop;

// Platform DLL management functions (internal - called by application layer)
CORE_API void platformLoadGame(const char* dllName);
CORE_API void platformUnloadGame();
CORE_API bool platformReloadGame(const char* dllName);

// ============================================================================
// PUBLIC GAME API - Safe for game code to call
// ============================================================================

// Utility functions for memory operations
CORE_API void memSet(void* dst, int value, size_t size);
CORE_API void memCopy(void* dst, const void* src, size_t size);