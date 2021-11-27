/**
 * @file rs232.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A simple, write-only driver for the RS232 serial
 * device standard. Code mostly ported from Panix-Archive (v2).
 * @version 0.3
 * @date 2020-06-29
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#define RS_232_COM1 0x3F8
#define RS_232_COM2 0x2F8
#define RS_232_COM3 0x3E8
#define RS_232_COM4 0x2E8

namespace RS232 {

/**
 * @brief Activates the RS232 serial driver
 *
 */
void init(uint16_t com_id);

/**
 * @brief Reads bytes from the serial buffer
 *
 * @param buf Buffer to hold the serial input
 * @param count Number of bytes to read
 * @return char* Returns the number of bytes read.
 */
size_t read(char* buf, size_t count);

/**
 * @brief Write bytes to the serial device
 *
 * @param buf Buffer containing bytes to write
 * @param count Number of bytes to write
 * @return size_t Returns number of bytes written
 */
size_t write(const char* buf, size_t count);

/**
 * @brief Prints a formatted string to serial output
 *
 * @param format Format string
 * @param ... Arguments
 * @return int Number of characters printed
 */
__attribute__ ((format (printf, 1, 2)))
int printf(const char *format, ...);

/**
 * @brief Prints a formatted string to serial output using
 * a va_list of arguments
 *
 * @param fmt Format string
 * @param args Arguments list
 * @return int Number of characters printed
 */
__attribute__ ((format (printf, 1, 0)))
int vprintf(const char* fmt, va_list args);

/**
 * @brief Closes the serial input buffer and frees all of
 * the data contained within.
 *
 * @return int Returns 0 on success and -1 on error. Errno
 * is set appropriately.
 */
int close();

} // !namespace RS232
