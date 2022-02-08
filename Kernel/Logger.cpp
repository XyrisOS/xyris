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
    RAIIMutex(the().m_logBufferMutex);
    ksprintf(the().m_logBuffer, "[%s] [%s] %s\n", tag, levelToString(lvl), fmt);
    LogHelperPrint(m_logBuffer, ap);
}

void Logger::LogHelperPrint(const char* fmt, va_list ap)
{
    for (size_t i = 0; i < m_writersIdx; i++) {
        if (the().m_writers[i] != nullptr) {
            the().m_writers[i](fmt, ap);
        }
    }
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
    : m_logBufferMutex("Logger")
    , m_writersIdx(0)
    , m_logLevel(lINFO)
{
    // Default constructor
}

// Kernel argument callback
static void argumentCallback(const char* lvl)
{
    // FIXME: Update kernel argument parser to handle `=`
    (void)lvl;
}

KERNEL_PARAM(logLevelArg, "--log-level=", argumentCallback);
