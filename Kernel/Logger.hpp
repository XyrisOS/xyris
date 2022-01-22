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

namespace Log {

enum LogLevel {
    lTRACE,
    lVERBOSE,
    lDEBUG,
    lINFO,
    lWARNING,
    lERROR,
};

typedef int (*LogWriter)(const char* fmt, va_list args);

[[gnu::format(printf, 2, 3)]] void Trace(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] void Verbose(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] void Debug(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] void Info(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] void Warning(const char* tag, const char* fmt, ...);
[[gnu::format(printf, 2, 3)]] void Error(const char* tag, const char* fmt, ...);

bool addWriter(LogWriter writer);
bool removeWriter(LogWriter writer);

void setLevel(LogLevel level);
LogLevel getLevel();

} // !namespace Log
