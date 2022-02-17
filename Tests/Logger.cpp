#include <cstdio>
#include "Memory/Arch.hpp"

namespace Test {

#include <Logger.hpp>

const char* Logger::levelToString(LogLevel lvl)
{
    switch (lvl) {
        case lTRACE:
            return "\033[36mTRACE\033[0m";
        case lDEBUG:
            return "\033[96mDEBUG\033[0m";
        case lVERBOSE:
            return "\033[92mVERBOSE\033[0m";
        case lINFO:
            return "\033[94mINFO\033[0m";
        case lWARNING:
            return "\033[93mWARNING\033[0m";
        case lERROR:
            return "\033[91mERROR\033[0m";
        default:
            return "UNKNOWN";
    }
}

void Logger::LogHelper(const char* tag, LogLevel lvl, const char* fmt, va_list ap)
{
    sprintf(the().m_logBuffer, "[%s] [%s] %s\n", tag, levelToString(lvl), fmt);
    LogHelperPrint(m_logBuffer, ap);
}

void Logger::LogHelperPrint(const char* fmt, va_list ap)
{
    vprintf(fmt, ap);
}

void Logger::Trace(const char* tag, const char* fmt, ...)
{
    if (lTRACE >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lTRACE, fmt, ap);
        va_end(ap);
    }
}

void Logger::Verbose(const char* tag, const char* fmt, ...)
{
    if (lVERBOSE >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lVERBOSE, fmt, ap);
        va_end(ap);
    }
}

void Logger::Debug(const char* tag, const char* fmt, ...)
{
    if (lDEBUG >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lDEBUG, fmt, ap);
        va_end(ap);
    }
}

void Logger::Info(const char* tag, const char* fmt, ...)
{
    if (lINFO >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lINFO, fmt, ap);
        va_end(ap);
    }
}

void Logger::Warning(const char* tag, const char* fmt, ...)
{
    if (lWARNING >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lWARNING, fmt, ap);
        va_end(ap);
    }
}

void Logger::Error(const char* tag, const char* fmt, ...)
{
    if (lERROR >= getLevel()) {
        va_list ap;
        va_start(ap, fmt);
        the().LogHelper(tag, lERROR, fmt, ap);
        va_end(ap);
    }
}

void Logger::Print(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelperPrint(fmt, ap);
    va_end(ap);
}

bool Logger::addWriter(LogWriter writer)
{
    (void)writer;
    return false;
}

bool Logger::removeWriter(LogWriter writer)
{
    (void)writer;
    return false;
}

Logger& Logger::the()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_logBufferMutex("Logger")
    , m_writersIdx(0)
    , m_logLevel(lVERBOSE)
{
    // Default constructor
}

}
