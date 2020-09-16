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
#include <dev/serial/rs232.hpp>
#include <mem/heap.hpp>

static px_ring_buff_t* read_buffer = NULL;
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
    // Grab the input character
    char in = px_rs232_read_char();
    // Change carriage returns to newlines
    if (in == '\r') {
        in = '\n';
    }
    // Create a string and print it so that the
    // user can see what they're typing.
    char str[2] = {in, '\0'};
    px_rs232_print(str);
    // Add the character to the circular buffer
    px_ring_buffer_enqueue(read_buffer, (uint8_t)in);
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

px_ring_buff_t* px_rs232_init_buffer(int size) {
    // Allocate space for the input buffer
    read_buffer = (px_ring_buff_t*)malloc(sizeof(px_ring_buff_t));
    // Initialize the ring buffer
    if (px_ring_buffer_init(read_buffer, size) == 0) {
        return read_buffer;
    } else {
        return NULL;
    }
}

px_ring_buff_t* px_rs232_get_buffer() {
    // Can return NULL. This is documented.
    return read_buffer;
}

char px_rs232_get_char() {
    // Grab the last byte and convert to a char
    uint8_t data;
    px_ring_buffer_dequeue(read_buffer, &data);
    return (char)data;
}

char* px_rs232_get_str() {
    int idx = 0;
    char* str = (char*)malloc(sizeof(char) * 1024);
    // Keep reading until the buffer is empty or
    // a newline is read.
    while (!px_ring_buffer_is_empty(read_buffer)) {
        uint8_t byte;
        px_ring_buffer_dequeue(read_buffer, &byte);
        str[idx] = (char)byte;
        ++idx;
        // Break if it's a newline
        if ((char)byte == '\n') {
            break;
        }
    }
    // Add the null terminator
    str[idx] = '\0';
    // Return the string
    return str;
}

int px_rs232_close() {
    int ret = -1;
    if (read_buffer != NULL) {
        ret = px_ring_buffer_destroy(read_buffer);
    }
    return ret;
}