#ifndef MY_STRING
#define MY_STRING

#include "arena.hpp"

struct String8{
    char* str;
    uint64_t size;
};

#define String8Lit(s) String8{s, sizeof(s) - 1}
#define Str8VArg(s) (int)(s).size, (s).str

CORE_API String8 cStringFromString8(Arena* arena, String8 string);
CORE_API String8 string8FromCString(char* string);
CORE_API String8 pushString8FV(Arena* arena, char* fmt, ...); //Format Vector
CORE_API String8 pushString8F(Arena* arena, char* fmt, ...); //Format

#endif