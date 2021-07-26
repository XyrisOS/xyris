/**
 * @file tty.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief TTY is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * kprintf().
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

enum tty_vga_color : uint32_t {
    VGA_Black           = 0x000000,
    VGA_Blue            = 0x0000AA,
    VGA_Green           = 0x00AA00,
    VGA_Cyan            = 0x00AAAA,
    VGA_Red             = 0xAA0000,
    VGA_Magenta         = 0xAA00AA,
    VGA_Brown           = 0xAA5500,
    VGA_LightGrey       = 0xAAAAAA,
    VGA_DarkGrey        = 0x555555,
    VGA_LightBlue       = 0x5555FF,
    VGA_LightGreen      = 0x55FF55,
    VGA_LightCyan       = 0x55FFFF,
    VGA_LightRed        = 0xFF5555,
    VGA_LightMagenta    = 0xFF55FF,
    VGA_Yellow          = 0xFFFF55,
    VGA_White           = 0xFFFFFF,
};

// Coorinate trackers
extern uint8_t tty_coords_x;
extern uint8_t tty_coords_y;
// VGA colors (defaults are white on black)
extern tty_vga_color color_back;
extern tty_vga_color color_fore;
// Default colors set by tty_clear()
extern tty_vga_color reset_back;
extern tty_vga_color reset_fore;

namespace console {

    void Write(char* str, uint32_t fore = VGA_White, uint32_t back = VGA_Black);

}
