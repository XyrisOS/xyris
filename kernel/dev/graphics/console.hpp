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
#include <arch/arch.hpp>

#define VGA_DEFAULT_BACK VGA_Black
#define VGA_DEFAULT_FORE VGA_White

#define DBG_INFO "[ \033[37mINFO \033[0m] "
#define DBG_WARN "[ \033[93mWARN \033[0m] "
#define DBG_FAIL "[ \033[91mFAIL \033[0m] "
#define DBG_OKAY "[ \033[92m OK  \033[0m] "

namespace Console {

int printf(const char* fmt, ...);
void write(const char c);
void write(const char* str);

}
