/**
 * @file console.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Framebuffer console
 * @version 0.3
 * @date 2019-09-26
 *
 * @copyright Copyright Keeton Feavel (c) 2019
 *
 */
#pragma once

#include <stdint.h>

#define VGA_DEFAULT_BACK VGA_Black
#define VGA_DEFAULT_FORE VGA_White

namespace Console {

void write(const char c);
void write(const char* str);
int printf(const char* fmt, ...);
void reset(uint32_t fore, uint32_t back);
void reset();

}
