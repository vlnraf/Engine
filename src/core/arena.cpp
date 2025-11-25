#include "arena.hpp"
#include "tracelog.hpp"

//--------------------------------------------------- Arena library --------------------------------------------------------------
Arena initArena(uint64_t memorySize){
    //Arena* arena = (Arena*) malloc(sizeof(Arena));
    Arena arena = {};
    arena.memory = (uint8_t*)malloc(memorySize);
    arena.index = 0;
    arena.size = memorySize;
    return arena;
}

bool isPowerOfTwo(uintptr_t x) {
	return (x & (x-1)) == 0;
}

uintptr_t alignForward(uintptr_t ptr, size_t align) {
	uintptr_t p, a, modulo;

	//assert(isPowerOfTwo(align));

	p = ptr;
	a = (uintptr_t)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a-1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}


void clearArena(Arena* arena){
    arena->index = 0;
}

void destroyArena(Arena* arena){
    free(arena->memory);
    //free(arena);
    arena->memory = NULL;
}

void* arenaAllocAligned(Arena* arena, uint64_t size, uint64_t align){
    uintptr_t currentAddr = (uintptr_t)arena->memory + arena->index;
    uintptr_t offset = alignForward(currentAddr, align);
    offset -= (uintptr_t)arena->memory;

    if(offset+size <= arena->size){
        void* ptr = &arena->memory[offset];
        arena->index = offset+size;
        return ptr;
    }
    return NULL;
}

void* arenaAlloc(Arena* arena, uint64_t size, uint64_t align){
    return arenaAllocAligned(arena, size, align);
}

void* arenaAllocAlignedZero(Arena* arena, uint64_t size, uint64_t align){
    void* result = (void*)arenaAllocAligned(arena, size, align);
    if(result){
        memSet(result, 0, size);
    }else{
        LOGERROR("Arena not allocating");
    }
    return result;
}

void* arenaAllocZero(Arena* arena, uint64_t size, uint64_t align){
    return arenaAllocAlignedZero(arena, size, align);
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

TempArena getTempArena(Arena* arena){
    TempArena temp = {};
    temp.arena = arena;
    temp.currOffset = arena->index;
    return temp;
}

void releaseTempArena(TempArena temp){
    temp.arena->index = temp.currOffset;
}