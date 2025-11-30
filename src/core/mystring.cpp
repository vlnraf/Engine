#include "mystring.hpp"

#include "stb_sprintf.h"

String8 cStringFromString8(Arena* arena, String8 string){
    String8 result = {0};
    result.size = string.size;
    result.str = arenaAllocArray(arena, char, string.size + 1); // +1 to produce a null terminated string
    memCopy(result.str, string.str, string.size);
    result.str[string.size] = 0;
    return result;
}

String8 string8FromCString(char* string){
    String8 result = {0};
    uint64_t size = 0;
    for(int i = 0; string[i] != '\0'; i++){
        size++;
    }
    result.size = size;
    result.str = string;
    return result;
}

String8 pushString8FV(Arena* arena, char* fmt, va_list args){
    String8 result = {0};
    va_list args2;
    va_copy(args2, args);
    uint64_t charNeeded = stbsp_vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args2);
    result.str = arenaAllocArrayZero(arena, char, charNeeded);
    result.size = charNeeded + 1;
    stbsp_vsnprintf(result.str, charNeeded, fmt, args2);
    return result;
}

String8 pushString8F(Arena* arena, char* fmt, ...){
    String8 result = {0};

    va_list args;
    va_start(args, fmt);
    result = pushString8FV(arena, fmt, args);
    va_end(args);
    return result;
}