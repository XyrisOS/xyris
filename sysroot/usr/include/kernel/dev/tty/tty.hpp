/**
 * @file px_tty.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief TTY is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * px_kprintf().
 * @version 0.3
 * @date 2019-09-26
 *
 * @copyright Copyright Keeton Feavel (c) 2019
 *
 */
#ifndef PANIX_PX_KPRINT_HPP
#define PANIX_PX_KPRINT_HPP

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
enum px_tty_vga_color {
    VGA_Black           = 0,
    VGA_Blue            = 1,
    VGA_Green           = 2,
    VGA_Cyan            = 3,
    VGA_Red             = 4,
    VGA_Magenta         = 5,
    VGA_Brown           = 6,
    VGA_LightGrey       = 7,
    VGA_DarkGrey        = 8,
    VGA_LightBlue       = 9,
    VGA_LightGreen      = 10,
    VGA_LightCyan       = 11,
    VGA_LightRed        = 12,
    VGA_LightMagenta    = 13,
    VGA_Yellow          = 14,
    VGA_White           = 15
};
/**
 * @brief ANSI color codes for use in functions
 * like px_kprintf(). However, the real printing
 * is done in the putchar() function. To change
 * the color from the foreground to the background,
 * add 10 to the desired color value.
 * (i.e. ANSI_Red == 31 (fore)--> 41 (back))
 * 
 */
enum px_tty_ansi_color {
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
    ANSI_BrightWhite    = 97
};
/**
 * @brief Clears the TTY and resets the cursor position.
 *
 */
void px_tty_clear(px_tty_vga_color fore = VGA_DEFAULT_FORE, px_tty_vga_color back = VGA_DEFAULT_BACK);
/**
 * @brief Resets the default TTY background and foreground
 * colors without clearing the screen.
 * 
 */
void px_tty_reset_defaults();
/**
 * @brief Sets the indicator in the top right corner.
 * Used mostly for debugging interrupts.
 *
 * @param color Indicator color
 */
void px_set_indicator(px_tty_vga_color color);
/**
 * @brief Prints a single character to the kernel display.
 *
 * @param c Character to be printed.
 */
int putchar(char c);
/**
 * @brief Prints a given string to the kernel display.
 * 
 * @param str String to be printed.
 */
int puts(const char *str);

#endif /* PANIX_px_kprint_HPP */
