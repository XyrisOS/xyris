/**
 * @file rs232.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A simple, write-only driver for the RS232 serial
 * device standard. Code mostly ported from Panix v2.
 * @version 0.3
 * @date 2020-06-29
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */
#pragma once

#include <stdarg.h>
#include <stdint.h>

#define RS_232_COM1 0x3F8
#define RS_232_COM2 0x2F8
#define RS_232_COM3 0x3E8
#define RS_232_COM4 0x2E8

#define RS_232_COM1_IRQ 0x04
#define RS_232_COM3_IRQ 0x04
#define RS_232_COM2_IRQ 0x03
#define RS_232_COM4_IRQ 0x03

#define RS_232_DATA_REG 0x0
#define RS_232_INTERRUPT_ENABLE_REG 0x1
#define RS_232_INTERRUPT_IDENTIFICATION_REG 0x2
#define RS_232_LINE_CONTROL_REG 0x3
#define RS_232_MODEM_CONTROL_REG 0x4
#define RS_232_LINE_STATUS_REG 0x5
#define RS_232_MODEM_STATUS_REG 0x6
#define RS_232_SCRATCH_REG 0x7

/**
 * @brief Activates the RS232 serial driver
 *
 */
void rs232_init(uint16_t com_id);

/**
 * @brief Prints a given character string to the
 * serial output.
 * @param str Input string to be printed.
 */
void rs232_print(const char* str);

/**
 * @brief Reads bytes from the serial buffer
 *
 * @param buf Buffer to hold the serial input
 * @param count Number of bytes to read
 * @return char* Returns the number of bytes read.
 */
size_t rs232_read(char* buf, size_t count);

/**
 * @brief Write bytes to the serial device
 *
 * @param buf Buffer containing bytes to write
 * @param count Number of bytes to write
 * @return size_t Returns number of bytes written
 */
size_t rs232_write(const char* buf, size_t count);

/**
 * @brief Closes the serial input buffer and frees all of
 * the data contained within.
 *
 * @return int Returns 0 on success and -1 on error. Errno
 * is set appropriately.
 */
int rs232_close();

int rs232_vprintf(const char* fmt, va_list args);

int rs232_printf(const char *format, ...);
