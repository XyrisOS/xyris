/**
 * @file px_tty.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief px_kprint is a small library to print unformatted strings to
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
#ifndef PANIX_px_kprint_HPP
#define PANIX_px_kprint_HPP

#include <sys/panix.hpp>
#include <arch/arch.hpp>

enum px_tty_color {
    Black           = 0x0,
    Blue            = 0x1,
    Green           = 0x2,
    Cyan            = 0x3,
    Red             = 0x4,
    Magenta         = 0x5,
    Brown           = 0x6,
    LightGrey       = 0x7,
    DarkGrey        = 0x8,
    LightBlue       = 0x9,
    LightGreen      = 0xA,
    LightCyan       = 0xB,
    LightRed        = 0xC,
    LightMagenta    = 0xD,
    Yellow          = 0xE,
    White           = 0xF
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
 * @brief Prints a given string to the kernel display.
 *
 * @param str Input string to be printed.
 */
void px_kprint(const char* str);
/**
 * @brief Prints a hexidecimal address to the kernel console.
 *
 * @param key Hexidecimal value to print.
 */
void px_kprint_hex(uint32_t key);
/**
 * @brief Prints out a string in a specified color
 *
 * @param str Input string to be printed
 * @param color Text color
 */
void px_kprint_color(char* str, px_tty_color color);
/**
 * @brief Set the color of the text when the next px_kprint is called.
 *
 * @param fore Foreground color
 * @param back Background color
 */
void px_tty_set_color(px_tty_color fore, px_tty_color back);
/**
 * @brief Clears the TTY and resets the cursor position.
 * 
 */
void px_clear_tty();
/**
 * @brief Sets the indicator in the top right corner.
 * Used mostly for debugging interrupts.
 * 
 * @param color Indicator color
 */
void px_set_indicator(px_tty_color color);

#endif /* PANIX_px_kprint_HPP */