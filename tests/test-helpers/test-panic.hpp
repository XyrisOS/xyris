#pragma once
#include <stdint.h>

void panic(const char* msg, const char* file, uint32_t line, const char* func);
