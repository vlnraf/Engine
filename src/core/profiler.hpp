#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "core/coreapi.hpp"

#define PROFILER_ON
#ifdef PROFILER_ON
#define PROFILER_SAVE(name) initProfiler(name);
#define PROFILER_START() startProfiling(__FUNCTION__);
#define PROFILER_END() endProfiling()
#define PROFILER_SCOPE_START(scopeName) startProfiling(scopeName);
#define PROFILER_SCOPE_END() endProfiling();
#define PROFILER_CLEANUP() destroyProfiler();
#else
#define PROFILER_SAVE(name)
#define PROFILER_START()
#define PROFILER_END()
#define PROFILER_SCOPE_START(scopeName)
#define PROFILER_SCOPE_END()
#define PROFILER_CLEANUP()
#endif

typedef struct{
    const char* name;
    float startTime;
    float endTime;
    int profilerCounter;
    uint32_t stateIndex;
} ProfilerState;

typedef struct{
    const char* fileName;
    FILE* profilerFile;
    ProfilerState* profilerState;
    uint32_t profilerIndex;
    uint32_t profilerCounter;
} MyProfiler;


// profiler.h
//extern MyProfiler prof;

CORE_API void initProfiler(const char* fileName);
CORE_API void startProfiling(const char* name);
CORE_API void endProfiling();
CORE_API void destroyProfiler();