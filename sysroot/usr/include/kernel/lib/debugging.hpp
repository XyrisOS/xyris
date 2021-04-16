#pragma once
#include <dev/serial/rs232.hpp>

#define __DBG_MSG(level, msg, ...) px_rs232_printf(level ": %s:%s: " msg "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WARN(...) __DBG_MSG("WARN", __VA_ARGS__)
#define ERR(...)  __DBG_MSG("ERR ", __VA_ARGS__)
#define MSG(...)  __DBG_MSG("MSG ", __VA_ARGS__)