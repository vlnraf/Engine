#include "tracelog.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>


void traceLog(LogLevel level, const char* fname, const int line, const char* message, ...){
    const char* logLevel[3] = {"[INFO]","[WARN]","[ERROR]"};

    struct tm timeinfo;
    time_t t = time(NULL);
    localtime_s(&timeinfo, &t);
    char timebuffer[9]; //HH:MM::SS
    strftime(timebuffer, sizeof(timebuffer), "%H:%M:%S", &timeinfo); 
    printf("[ %s ] %s [ FILE: %s:%d ]: ", timebuffer, logLevel[level], fname, line);
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    printf("\n");
}