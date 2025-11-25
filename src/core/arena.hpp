#pragma once

#include <stdint.h>
#include <memory.h>
#include <malloc.h>

#include "core/coreapi.hpp"
#include "platform/platform.hpp"

#define Max(a, b) (((a)>(b)) ? (a) : (b))
#define alignOf(T) __alignof(T)
#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif
#define KB(x) ((uint64_t)(x) * 1024)
#define MB(x) ((uint64_t)(KB(x)) * 1024)
#define GB(x) ((uint64_t)(MB(x)) * 1024)
#define DEFAULT_SIZE MB(4)

struct Arena{
    uint8_t* memory;
    uint64_t index;
    uint64_t size;
};

struct TempArena{
    Arena* arena;
    uint64_t currOffset;
};

#define arenaAllocStruct(arena, T) (T*)arenaAlloc(arena, sizeof(T), Max(8, alignOf(T)))
#define arenaAllocArray(arena, T, count) (T*)arenaAlloc(arena, sizeof(T) * (count), Max(8, alignOf(T)))
#define arenaAllocStructZero(arena, T) (T*)arenaAllocZero(arena, sizeof(T), Max(8, alignOf(T)))
#define arenaAllocArrayZero(arena, T, count) (T*)arenaAllocZero(arena, sizeof(T) * (count), Max(8, alignOf(T)))

CORE_API Arena initArena(uint64_t memorySize = DEFAULT_SIZE);

CORE_API void clearArena(Arena* arena);
CORE_API void destroyArena(Arena* arena);
CORE_API void* arenaAllocAligned(Arena* arena, uint64_t size, uint64_t align);
CORE_API void* arenaAlloc(Arena* arena, uint64_t size, uint64_t align);
CORE_API void* arenaAllocAlignedZero(Arena* arena, uint64_t size, uint64_t align);
CORE_API void* arenaAllocZero(Arena* arena, uint64_t size, uint64_t align);
CORE_API TempArena getTempArena(Arena* arena);
CORE_API void releaseTempArena(TempArena temp);
CORE_API uint64_t arenaGetPos(Arena* arena);
CORE_API uint64_t arenaGetMemoryUsed(Arena* arena);
CORE_API uint64_t arenaGetMemoryLeft(Arena* arena);