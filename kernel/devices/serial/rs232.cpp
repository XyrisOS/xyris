/**
 * @file rs232.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-06-29
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#include <arch/arch.hpp>
#include <devices/serial/rs232.hpp>

char* lineBuffer[RS_232_BUF_SIZE];
uint8_t rs_232_line_index;
uint16_t rs_232_port_base;

static int px_rs232_received();
static int px_rs232_is_transmit_empty();
static char px_rs232_read_char();
static void px_rs232_write_char(char c);
static void px_rs232_callback(registers_t *regs);
void px_rs232_init(uint16_t port_num);

static int px_rs232_received() {
    return px_read_byte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 1;
}

static int px_rs232_is_transmit_empty() {
    return px_read_byte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 0x20;
}

static char px_rs232_read_char() {
    while (px_rs232_received() == 0);
    return px_read_byte(rs_232_port_base + RS_232_DATA_REG);
}

static void px_rs232_write_char(char c) {
    while (px_rs232_is_transmit_empty() == 0);
    px_write_byte(rs_232_port_base + RS_232_DATA_REG, c);
}

void px_rs232_print(char* str) {
    for (int i = 0; str[i] != 0; i++) {
        px_rs232_write_char(str[i]);
    }
}

static void px_rs232_callback(registers_t *regs) {
    char str[2] = { px_rs232_read_char(), '\0' };
    if (str[0] == '\r') {
        px_rs232_print("\n");
    }
    px_rs232_print(str);
}

void px_rs232_init(uint16_t com_id) {
    // Register the IRQ callback
    rs_232_port_base = com_id;
    uint8_t IRQ = 0x20 + (com_id == RS_232_COM1 ? RS_232_COM1_IRQ : RS_232_COM2_IRQ);
    px_register_interrupt_handler(IRQ, px_rs232_callback);
    // Write the port data to activate the device
    // disable interrupts
    px_write_byte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x00);
    px_write_byte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x80);
    px_write_byte(rs_232_port_base + RS_232_DATA_REG, 0x03);
    px_write_byte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x00);
    px_write_byte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x03);
    px_write_byte(rs_232_port_base + RS_232_INTERRUPT_IDENTIFICATION_REG, 0xC7);
    px_write_byte(rs_232_port_base + RS_232_MODEM_CONTROL_REG, 0x0B);
    px_write_byte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x00);
    // re-enable interrupts
    px_write_byte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x01);
    // Print out header info to the serial
    px_rs232_print(
        "\033[93m\n    ____              _              _____\n"
        "   / __ \\____ _____  (_)  __   _   _|__  /\n"
        "  / /_/ / __ `/ __ \\/ / |/_/  | | / //_ < \n"
        " / ____/ /_/ / / / / />  <    | |/ /__/ / \n"
        "/_/    \\__,_/_/ /_/_/_/|_|    |___/____/  \n\n\033[0m"
        "Panix Serial Output Debugger\n\n"
    );
}
