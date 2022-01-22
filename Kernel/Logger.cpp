/**
 * @file Logger.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-01-21
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include "Logger.hpp"
#include <Bootloader/Arguments.hpp>
#include <Library/stdio.hpp>
#include <Library/mutex.hpp>

#define MAX_LOG_SIZE 1024
#define MAX_WRITER_COUNT 2

namespace Log {

// static char logBuffer[MAX_LOG_SIZE];
static Mutex logBufferMutex("LogBuffer");
static LogWriter writers[MAX_WRITER_COUNT];
static size_t writersIdx = 0;
static LogLevel logLevel = lINFO;

static const char* levelToString(LogLevel lvl)
{
    switch (lvl) {
        case lTRACE:
            return "TRACE";
        case lDEBUG:
            return "DEBUG";
        case lVERBOSE:
            return "VERBOSE";
        case lINFO:
            return "INFO";
        case lWARNING:
            return "WARNING";
        case lERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

static void LogHelper(const char* tag, const char* fmt, va_list args)
{
    (void)tag;
    (void)fmt;
    (void)args;
    if (logBufferMutex.Lock() == 0) {
        // TODO
        logBufferMutex.Unlock();
    }
}

void Trace(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

void Verbose(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

void Debug(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

void Info(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

void Warning(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

void Error(const char* tag, const char* fmt, ...)
{
    (void)tag;
    (void)fmt;
}

bool addWriter(LogWriter writer)
{
    if (writersIdx < MAX_WRITER_COUNT) {
        writers[writersIdx++] = writer;
        return true;
    }

    return false;
}

bool removeWriter(LogWriter writer)
{
    for (size_t idx = 0; idx < writersIdx; idx++) {
        if (writers[idx] == writer) {
            writers[idx] = nullptr;
            return true;
        }
    }

    return false;
}

void setLevel(LogLevel level)
{
    logLevel = level;
}

LogLevel getLevel()
{
    return logLevel;
}


// Kernel argument callback
static void argumentCallback(const char* lvl)
{
    (void)lvl;
}

KERNEL_PARAM(logLevelArg, "--log-level=", argumentCallback);

} // !namespace Log
