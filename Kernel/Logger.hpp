/**
 * @file Logger.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-01-21
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <stdarg.h>
#include <Locking/Mutex.hpp>
#include <Locking/RAII.hpp>

class Logger
{
public:

enum LogLevel {
    lTRACE,
    lDEBUG,
    lVERBOSE,
    lINFO,
    lWARNING,
    lERROR,
    lNone,
};

typedef int (*LogWriter)(const char* fmt, va_list args);

[[gnu::format(printf, 2, 3)]] static void Trace(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] static void Verbose(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] static void Debug(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] static void Info(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] static void Warning(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] static void Error(const char* tag, const char* fmt, ...);

static bool addWriter(LogWriter writer);
static bool removeWriter(LogWriter writer);

static void setLevel(LogLevel level) { the().m_logLevel = level; }
static LogLevel getLevel() { return the().m_logLevel; }

static Logger& the();

private:
    Logger();
    const char* levelToString(LogLevel lvl);
    void LogHelper(const char* tag, LogLevel lvl, const char* fmt, va_list args);

    static const uint8_t m_maxWriterCount = 2;
    static const uint32_t m_maxBufferSize = 1024;
    Mutex m_logBufferMutex;
    size_t m_writersIdx;
    LogLevel m_logLevel;
    LogWriter m_writers[m_maxWriterCount];
    char m_logBuffer[m_maxBufferSize];
};
