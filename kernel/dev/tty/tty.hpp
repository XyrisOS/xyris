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

/**
 * @brief Binaries values which change the BIOS
 * VGA colors when written to memory. These value
 * are not regularly used now that putchar() has
 * support for ANSI color codes. A translation
 * table is used to convert between ANSI and VGA
 * in tty.cpp.
 *
 */
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
    VGA_Yellow          = 0x5555FF,
    VGA_White           = 0xFFFFFF,
};
/**
 * @brief ANSI color codes for use in functions
 * like kprintf(). However, the real printing
 * is done in the putchar() function. To change
 * the color from the foreground to the background,
 * add 10 to the desired color value.
 * (i.e. ANSI_Red == 31 (fore)--> 41 (back))
 *
 */
enum tty_ansi_color : uint16_t {
    ANSI_Black          = 30,
    ANSI_Red            = 31,
    ANSI_Green          = 32,
    ANSI_Yellow         = 33,
    ANSI_Blue           = 34,
    ANSI_Magenta        = 35,
    ANSI_Cyan           = 36,
    ANSI_White          = 37,
    ANSI_BrightBlack    = 90,
    ANSI_BrightRed      = 91,
    ANSI_BrightGreen    = 92,
    ANSI_BrightYellow   = 93,
    ANSI_BrightBlue     = 94,
    ANSI_BrightMagenta  = 95,
    ANSI_BrightCyan     = 96,
    ANSI_BrightWhite    = 97,
};
#define VGA_COLOR(bg, fg) (uint16_t)(((bg)<<4)|((fg)&0xF))
#define VGA_CHAR(ch, co) (uint16_t)((ch)|((co)<<8))
// Coorinate trackers
extern uint8_t tty_coords_x;
extern uint8_t tty_coords_y;
// VGA colors (defaults are white on black)
extern tty_vga_color color_back;
extern tty_vga_color color_fore;
// Default colors set by tty_clear()
extern tty_vga_color reset_back;
extern tty_vga_color reset_fore;
/**
 * @brief Shifts the entire TTY screen up by one line.
 *
 */
void tty_shift_up();
/**
 * @brief Clears the TTY and resets the cursor position.
 *
 */
void tty_clear(tty_vga_color fore = VGA_DEFAULT_FORE, tty_vga_color back = VGA_DEFAULT_BACK);
/**
 * @brief Resets the default TTY background and foreground
 * colors without clearing the screen.
 *
 */
void tty_reset_defaults();
/**
 * @brief Sets the indicator in the top right corner.
 * Used mostly for debugging interrupts.
 *
 * @param color Indicator color
 */
void set_indicator(tty_vga_color color);
