#pragma once

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
    #ifdef CORE_EXPORT
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#else
    #define CORE_API
#endif