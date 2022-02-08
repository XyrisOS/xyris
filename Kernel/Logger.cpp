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

const char* Logger::levelToString(LogLevel lvl)
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

void Logger::LogHelper(const char* tag, LogLevel lvl, const char* fmt, va_list ap)
{
    (void)tag;
    (void)lvl;
    RAIIMutex(the().m_logBufferMutex);
    if (the().m_logBufferMutex.lock() == 0) {
        kvsprintf(the().m_logBuffer, fmt, ap);
        m_logBufferMutex.unlock();
    }
}

void Logger::Trace(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lTRACE, fmt, ap);
    va_end(ap);
}

void Logger::Verbose(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lVERBOSE, fmt, ap);
    va_end(ap);
}

void Logger::Debug(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lDEBUG, fmt, ap);
    va_end(ap);
}

void Logger::Info(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lINFO, fmt, ap);
    va_end(ap);
}

void Logger::Warning(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lWARNING, fmt, ap);
    va_end(ap);
}

void Logger::Error(const char* tag, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    the().LogHelper(tag, lERROR, fmt, ap);
    va_end(ap);
}

bool Logger::addWriter(LogWriter writer)
{
    if (the().m_writersIdx < the().m_maxWriterCount) {
        the().m_writers[the().m_writersIdx++] = writer;
        return true;
    }

    return false;
}

bool Logger::removeWriter(LogWriter writer)
{
    for (size_t idx = 0; idx < the().m_writersIdx; idx++) {
        if (the().m_writers[idx] == writer) {
            the().m_writers[idx] = nullptr;
            return true;
        }
    }

    return false;
}

Logger& Logger::the()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_logBufferMutex("LogBuffer")
    , m_writersIdx(0)
    , m_logLevel(lINFO)
{
    // Default constructor
}


// Kernel argument callback
static void argumentCallback(const char* lvl)
{
    (void)lvl;
}

KERNEL_PARAM(logLevelArg, "--log-level=", argumentCallback);
