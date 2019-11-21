/**
 * @file kprint.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief kprint is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * kprintf().
 * @version 0.1
 * @date 2019-09-26
 * 
 * @copyright Copyright Keeton Feavel (c) 2019
 * 
 */
#ifndef PANIX_KPRINT_HPP
#define PANIX_KPRINT_HPP

#include <sys/sys.hpp>

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
// Updated the address since we moved to a higher-half kernel mapping
inline uint16_t* videoMemory = (uint16_t*) 0xC03FF000;
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
void kprint(const char* str);
/**
 * @brief Prints a single character to the kernel display.
 * 
 * @param character Character to be printed.
 */
void putchar(char character);
/**
 * @brief Prints a given string to a particular coordinate in the kernel display.
 * 
 * @param str Input string to be printed.
 * @param x X-coordinate of the kernel display.
 * @param y Y-coordinate of the kernel display.
 * @param resetCursor Determines whether or not the cursor should be reset to the starting position.
 */
void kprintAtPosition(const char* str, uint8_t x, uint8_t y, bool resetCursor = false);
/**
 * @brief Prints a hexidecimal address to the kernel console.
 * 
 * @param key Hexidecimal value to print.
 */
void kprintHex(uint8_t key);

/**
 * @brief Set the color of the text when the next kprint is called.
 * 
 * @param fore Foreground color
 * @param back Background color
 */
void kprintSetColor(px_tty_color fore, px_tty_color back);

void clearScreen();

#endif /* PANIX_KPRINT_HPP */