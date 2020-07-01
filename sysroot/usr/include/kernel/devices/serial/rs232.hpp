/**
 * @file rs232.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A simple, write-only driver for the RS232 serial
 * device standard. Code mostly ported from Panix v2.
 * @version 0.3
 * @date 2020-06-29
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */

#include <sys/panix.hpp>

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
void px_rs_232_init(uint16_t com_id);

/**
 * @brief Prints a given character string to the
 * serial output.
 * @param str Input string to be printed.
 */
void px_rs_232_print(char* str);