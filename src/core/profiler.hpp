#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "core/coreapi.hpp"

//#define PROFILER_ON
#ifdef PROFILER_ON
#define PROFILER_SAVE(name) initProfiler(&prof, name);
#define PROFILER_START() startProfiling(&prof, __FUNCTION__);
#define PROFILER_END() endProfiling(&prof)
#define PROFILER_SCOPE_START(scopeName) startProfiling(&prof, scopeName);
#define PROFILER_SCOPE_END() endProfiling(&prof);
#define PROFILER_CLEANUP() destroyProfiler(&prof);
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
extern MyProfiler prof;

CORE_API void initProfiler(MyProfiler* prof, const char* fileName);
CORE_API void startProfiling(MyProfiler* prof, const char* name);
CORE_API void endProfiling(MyProfiler* prof);
CORE_API void destroyProfiler(MyProfiler* prof);