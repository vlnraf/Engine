#include "arena.hpp"
#include "tracelog.hpp"

//--------------------------------------------------- Arena library --------------------------------------------------------------
Arena* initArena(uint64_t memorySize){
    //Arena arena = {};
    //arena.memory = malloc(memorySize);
    //arena.index = 0;
    //arena.previousIndex = 0;
    //arena.size = memorySize;
    Arena* arena = (Arena*) malloc(sizeof(Arena));
    arena->memory = malloc(memorySize);
    arena->index = 0;
    arena->previousIndex = 0;
    arena->size = memorySize;
    return arena;
}


void clearArena(Arena* arena){
    arena->index = 0;
    arena->previousIndex = 0;
}

void destroyArena(Arena* arena){
    free(arena->memory);
    free(arena);
    arena->memory = NULL;
}

void* arenaAllocAligned(Arena* arena, uint64_t size, uint32_t align){
    uintptr_t currentAddr = (uintptr_t)arena->memory + arena->index;
    uintptr_t alignedAddr = (currentAddr + (align - 1)) & ~(uint64_t)(align - 1);
    uint64_t padding = alignedAddr - currentAddr;
    if(arena->index + padding + size > arena->size){
        LOGERROR("arena out of boud");
        return NULL;
    }

    arena->index += padding + size;
    return (void*)alignedAddr;
}

void* arenaAlloc(Arena* arena, uint64_t size){
    return arenaAllocAligned(arena, size, DEFAULT_ALIGNMENT);
}

void* arenaAllocAlignedZero(Arena* arena, uint64_t size, uint32_t align){
    void* result = (void*)arenaAllocAligned(arena, size, align);
    if(result){
        memset(result, 0, size);
    }
    return result;
}

void* arenaAllocZero(Arena* arena, uint64_t size){
    return arenaAllocAlignedZero(arena, size, DEFAULT_ALIGNMENT);
}

uint64_t arenaGetPos(Arena* arena){
    return arena->index;
}

uint64_t arenaGetMemoryUsed(Arena* arena){
    return arena->index;
}

uint64_t arenaGetMemoryLeft(Arena* arena){
    return arena->size - arena->index;
}