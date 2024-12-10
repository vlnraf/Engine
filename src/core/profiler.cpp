#include <stdlib.h>

#include "profiler.hpp"
#include "tracelog.hpp"


void writeHeader(MyProfiler* prof);

void initProfiler(MyProfiler* prof, const char* fileName){
    prof->fileName = fileName;
    prof->profilerState = (ProfilerState*)malloc(sizeof(ProfilerState));
    prof->profilerFile = fopen(fileName, "w");
    prof->profilerCounter = 0;
    prof->profilerIndex = 0;
    if(!prof->profilerFile){
        LOGERROR("File opening Failed");
        exit(1);
    }
    writeHeader(prof);
}

void writeHeader(MyProfiler* prof){
    fprintf(prof->profilerFile, "{");
    fprintf(prof->profilerFile, "\"traceEvents\":[");
    fflush(prof->profilerFile);
}

void writeFooter(MyProfiler* prof){
    fprintf(prof->profilerFile, "],");
    fprintf(prof->profilerFile, "\"displayTimeUnit\":\"ms\"");
    fprintf(prof->profilerFile, "}");
    fflush(prof->profilerFile);
}

void startProfiling(MyProfiler* prof, const char* name){
    ProfilerState state = {};
    state.name = name;
    state.startTime = (float)clock() / CLOCKS_PER_SEC * 1000000;
    prof->profilerState = (ProfilerState*)realloc(prof->profilerState, sizeof(ProfilerState)*(prof->profilerIndex+1));
    prof->profilerState[prof->profilerIndex++] = state;
}

void writeProfiling(MyProfiler* prof, ProfilerState state){
    if(prof->profilerCounter++ > 0){
        fprintf(prof->profilerFile, ",");
        fflush(prof->profilerFile);
    }
    float elapsed_time = (float)(state.endTime - state.startTime);// / CLOCKS_PER_SEC * 1000;
    fprintf(prof->profilerFile, "{");
    fprintf(prof->profilerFile, "\"ph\":\"X\",");
    fprintf(prof->profilerFile, "\"cat\":\"cpu_op\",");
    fprintf(prof->profilerFile, "\"name\":\"%s\",", state.name); 
    fprintf(prof->profilerFile, "\"pid\":\"0\","); 
    fprintf(prof->profilerFile, "\"tid\":%d,", 0);//threadId); 
    fprintf(prof->profilerFile, "\"dur\": %f,", elapsed_time);
    fprintf(prof->profilerFile, "\"ts\": %f", state.startTime);
    fprintf(prof->profilerFile, "}");
    fflush(prof->profilerFile);
}

void endProfiling(MyProfiler* prof){
    ProfilerState state = prof->profilerState[--prof->profilerIndex];
    state.endTime = (float)clock() / CLOCKS_PER_SEC * 1000000;
    writeProfiling(prof, state);
    prof->profilerState = (ProfilerState*)realloc(prof->profilerState, sizeof(ProfilerState)*(prof->profilerIndex));
}

void destroyProfiler(MyProfiler* prof){
    writeFooter(prof);
    free(prof->profilerState);
    fclose(prof->profilerFile);
}