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
#include <lib/ring_buffer.hpp>

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

#define RS_232_BUF_SIZE 64

/**
 * @brief Activates the RS232 serial driver
 *
 */
void px_rs232_init(uint16_t com_id);

/**
 * @brief Prints a given character string to the
 * serial output.
 * @param str Input string to be printed.
 */
void px_rs232_print(const char* str);

/**
 * @brief Returns the most recently received byte
 * from the serial input.
 * @return char Character representation of latest
 * buffer byte.
 */
char px_rs232_get_char();

/**
 * @brief Returns all of the characters within the
 * input buffer up until a newline or a null terminator.
 * 
 * @param str Character buffer to hold the serial input
 * @param max Max number of characters to read in
 * @return char* Returns the number of characters read.
 */
int px_rs232_get_str(char* str, int max);

/**
 * @brief Closes the serial input buffer and frees all of
 * the data contained within.
 *
 * @return int Returns 0 on success and -1 on error. Errno
 * is set appropriately.
 */
int px_rs232_close();

int px_rs232_vprintf(const char* fmt, va_list args);

int px_rs232_printf(const char *format, ...);
