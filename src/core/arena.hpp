#pragma once

#include <stdint.h>
#include <memory.h>
#include <malloc.h>

#include "core/coreapi.hpp"

#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#define KB(x) ((uint64_t)(x) * 1024)
#define MB(x) ((uint64_t)(KB(x)) * 1024)
#define GB(x) ((uint64_t)(MB(x)) * 1024)
#define DEFAULT_SIZE MB(4)

struct Arena{
    void* memory;
    uint64_t index;
    uint64_t previousIndex;
    uint64_t size;
};

CORE_API Arena* initArena(uint64_t memorySize = DEFAULT_SIZE);

CORE_API void clearArena(Arena* arena);
CORE_API void destroyArena(Arena* arena);
CORE_API void* arenaAllocAligned(Arena* arena, uint64_t size, uint32_t align);
CORE_API void* arenaAlloc(Arena* arena, uint64_t size);
CORE_API void* arenaAllocAlignedZero(Arena* arena, uint64_t size, uint32_t align);
CORE_API void* arenaAllocZero(Arena* arena, uint64_t size);
#define arenaAllocStruct(arena, T) (T*)arenaAlloc(arena, sizeof(T))
#define arenaAllocArray(arena, T, count) (T*)arenaAlloc(arena, sizeof(T) * count)
#define arenaAllocStructZero(arena, T) (T*)arenaAllocZero(arena, sizeof(T))
#define arenaAllocArrayZero(arena, T, count) (T*)arenaAllocZero(arena, sizeof(T) * count)
CORE_API uint64_t arenaGetPos(Arena* arena);
CORE_API uint64_t arenaGetMemoryUsed(Arena* arena);
CORE_API uint64_t arenaGetMemoryLeft(Arena* arena);