#pragma once

#if defined(_WIN32)
    #ifdef CORE_EXPORT
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#else
    // Linux / macOS
    #ifdef CORE_EXPORT
        #define CORE_API __attribute__((visibility("default")))
    #else
        #define CORE_API
    #endif
#endif