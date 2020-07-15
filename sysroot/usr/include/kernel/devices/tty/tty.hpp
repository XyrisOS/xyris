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

#include <sys/panix.hpp>
#include <arch/arch.hpp>

#define VGA_DEFAULT_BACK VGA_Black
#define VGA_DEFAULT_FORE VGA_White

enum px_tty_vga_color {
    VGA_Black           = 0x0,
    VGA_Blue            = 0x1,
    VGA_Green           = 0x2,
    VGA_Cyan            = 0x3,
    VGA_Red             = 0x4,
    VGA_Magenta         = 0x5,
    VGA_Brown           = 0x6,
    VGA_LightGrey       = 0x7,
    VGA_DarkGrey        = 0x8,
    VGA_LightBlue       = 0x9,
    VGA_LightGreen      = 0xA,
    VGA_LightCyan       = 0xB,
    VGA_LightRed        = 0xC,
    VGA_LightMagenta    = 0xD,
    VGA_Yellow          = 0xE,
    VGA_White           = 0xF
};

// Add 10 to these to convert from
// foreground colors to background
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

enum px_print_level {
    Info            = 0,
    Warning         = 1,
    Error           = 2,
    Success         = 3
};

/**
 * @brief Prints a single character to the kernel display.
 *
 * @param c Character to be printed.
 */
void putchar(char c);
/**
 * @brief Prints a debug message to the kernel display and
 * sets a tag and color according to the debug level.
 *
 * @param msg Message to be printed
 * @param lvl Message debug level
 */
void px_print_debug(char* msg, px_print_level lvl);
/**
 * @brief Clears the TTY and resets the cursor position.
 *
 */
void px_clear_tty(px_tty_vga_color fore = VGA_DEFAULT_FORE, px_tty_vga_color back = VGA_DEFAULT_BACK);
/**
 * @brief Sets the indicator in the top right corner.
 * Used mostly for debugging interrupts.
 *
 * @param color Indicator color
 */
void px_set_indicator(px_tty_vga_color color);

#endif /* PANIX_px_kprint_HPP */
