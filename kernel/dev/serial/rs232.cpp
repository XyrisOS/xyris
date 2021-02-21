/**
 * @file rs232.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-06-29
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 * TODO: Make this module robust enough for fast transmission of binary data
 * currently it adequately supports slow, character transmission.
 *
 */

#include <arch/arch.hpp>
#include <dev/serial/rs232.hpp>
#include <mem/heap.hpp>
#include <lib/mutex.hpp>
#include <stdarg.h>
#include <lib/stdio.hpp>

static px_ring_buff_t* read_buffer = NULL;
uint8_t rs_232_line_index;
uint16_t rs_232_port_base;
px_mutex_t mutex_rs232("rs232");

static int _px_rs232_received();
static int _px_rs232_is_transmit_empty();
static char _px_rs232_read_char();
static void _px_rs232_write_char(char c);
static void _px_rs232_callback(registers_t *regs);

static int _px_rs232_received() {
    return px_read_byte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 1;
}

static int _px_rs232_is_transmit_empty() {
    return px_read_byte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 0x20;
}

static char _px_rs232_read_char() {
    px_mutex_lock(&mutex_rs232);
    while (_px_rs232_received() == 0);
    px_mutex_unlock(&mutex_rs232);
    return px_read_byte(rs_232_port_base + RS_232_DATA_REG);
}

static inline void _px_rs232_write_char(char c) {
    while (_px_rs232_is_transmit_empty() == 0);
    px_write_byte(rs_232_port_base + RS_232_DATA_REG, c);
}

static int vprintf_helper(unsigned c, void **ptr)
{
    (void) ptr;
    _px_rs232_write_char((char)c);
    return 0;
}

int px_rs232_vprintf(const char* fmt, va_list args)
{
    int retval;
    retval = px_do_printf(fmt, args, vprintf_helper, NULL);
    return retval;
}

int px_rs232_printf(const char *format, ...)
{
    va_list args;
    int ret_val;

    va_start(args, format);
    ret_val = px_rs232_vprintf(format, args);
    va_end(args);
    return ret_val;
}

void px_rs232_print(const char* str) {
    for (int i = 0; str[i] != 0; i++) {
        _px_rs232_write_char(str[i]);
    }
}

static void _px_rs232_callback(registers_t *regs) {
    (void)regs;
    // Grab the input character
    char in = _px_rs232_read_char();
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
    px_register_interrupt_handler(IRQ, _px_rs232_callback);
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
    px_mutex_lock(&mutex_rs232);
    // Allocate space for the input buffer
    read_buffer = (px_ring_buff_t*)malloc(sizeof(px_ring_buff_t));
    // Initialize the ring buffer
    if (px_ring_buffer_init(read_buffer, size) == 0) {
        px_mutex_unlock(&mutex_rs232);
        return read_buffer;
    } else {
        px_mutex_unlock(&mutex_rs232);
        return NULL;
    }
}

px_ring_buff_t* px_rs232_get_buffer() {
    // Can return NULL. This is documented.
    return read_buffer;
}

char px_rs232_get_char() {
    px_mutex_lock(&mutex_rs232);
    // Grab the last byte and convert to a char
    uint8_t data = 0;
    if (read_buffer != NULL)
    {
        px_ring_buffer_dequeue(read_buffer, &data);
    }
    px_mutex_unlock(&mutex_rs232);
    return (char)data;
}

int px_rs232_get_str(char* str, int max) {
    px_mutex_lock(&mutex_rs232);
    int idx = 0;
    // Keep reading until the buffer is empty or
    // a newline is read.
    while (!px_ring_buffer_is_empty(read_buffer)) {
        uint8_t byte;
        px_ring_buffer_dequeue(read_buffer, &byte);
        str[idx] = (char)byte;
        ++idx;
        // Break if it's a newline or null
        if ((char)byte == '\n'
        || byte == 0
        || idx == (max - 1))
        {
            // Add the null terminator
            str[idx] = '\0';
            ++idx;
            break;
        }
    }
    px_mutex_unlock(&mutex_rs232);
    // Return the string
    return idx;
}

int px_rs232_close() {
    px_mutex_lock(&mutex_rs232);
    int ret = -1;
    if (read_buffer != NULL) {
        ret = px_ring_buffer_destroy(read_buffer);
        read_buffer = NULL;
    }
    px_mutex_unlock(&mutex_rs232);
    return ret;
}
