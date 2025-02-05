#ifndef TRACE_LOG_H
#define TRACE_LOG_H

#include "coreapi.hpp"

typedef enum LogLevel{
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

CORE_API void traceLog(LogLevel logLevel, const char* fname, const int line, const char* message, ...);

#define LOGINFO(message, ...) traceLog(LOG_LEVEL_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__); //## Is used for a special case where you pass no arguments at all
#define LOGWARN(message, ...) traceLog(LOG_LEVEL_WARN, __FILE__, __LINE__, message, ##__VA_ARGS__);
#define LOGERROR(message, ...) traceLog(LOG_LEVEL_ERROR,__FILE__, __LINE__, message, ##__VA_ARGS__);

#endif