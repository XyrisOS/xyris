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

#include <stdarg.h>
#include <arch/arch.hpp>
#include <dev/serial/rs232.hpp>
#include <mem/heap.hpp>
#include <lib/stdio.hpp>
#include <lib/mutex.hpp>
#include <lib/string.hpp>
#include <lib/ring_buffer.hpp>

static uint8_t rs_232_data[1024];
static uint16_t rs_232_port_base;
static RingBuffer ring(sizeof(rs_232_data), rs_232_data);
static mutex_t mutex_rs232("rs232");

static int rs232_received();
static int rs232_is_transmit_empty();
static char rs232_read_char();
static void rs232_write_char(char c);
static void rs232_callback(registers_t *regs);

static int rs232_received() {
    return readByte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 1;
}

static int rs232_is_transmit_empty() {
    return readByte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 0x20;
}

static char rs232_read_char() {
    mutex_lock(&mutex_rs232);
    while (rs232_received() == 0);
    mutex_unlock(&mutex_rs232);
    return readByte(rs_232_port_base + RS_232_DATA_REG);
}

static inline void rs232_write_char(char c) {
    while (rs232_is_transmit_empty() == 0);
    writeByte(rs_232_port_base + RS_232_DATA_REG, c);
}

static int vprintf_helper(unsigned c, void **ptr)
{
    (void) ptr;
    rs232_write_char((char)c);
    return 0;
}

int rs232_vprintf(const char* fmt, va_list args)
{
    int retval;
    retval = do_printf(fmt, args, vprintf_helper, NULL);
    return retval;
}

int rs232_printf(const char *format, ...)
{
    va_list args;
    int ret_val;

    va_start(args, format);
    ret_val = rs232_vprintf(format, args);
    va_end(args);
    return ret_val;
}

void rs232_print(const char* str) {
    for (int i = 0; str[i] != 0; i++) {
        rs232_write_char(str[i]);
    }
}

static void rs232_callback(registers_t *regs) {
    (void)regs;
    // Grab the input character
    char in = rs232_read_char();
    // Change carriage returns to newlines
    if (in == '\r') {
        in = '\n';
    }
    // Create a string and print it so that the
    // user can see what they're typing.
    char str[2] = {in, '\0'};
    rs232_print(str);
    // Add the character to the circular buffer
    ring.Enqueue((uint8_t)in);
}

// FIXME: This should take a buffer as an argument so that
// COM1 & COM2 can have different buffers, or find a way
// to keep them separate.
void rs232_init(uint16_t com_id) {
    // Register the IRQ callback
    rs_232_port_base = com_id;
    uint8_t IRQ = 0x20 + (com_id == RS_232_COM1 ? RS_232_COM1_IRQ : RS_232_COM2_IRQ);
    register_interrupt_handler(IRQ, rs232_callback);
    // Write the port data to activate the device
    // disable interrupts
    writeByte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x00);
    writeByte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x80);
    writeByte(rs_232_port_base + RS_232_DATA_REG, 0x03);
    writeByte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x00);
    writeByte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x03);
    writeByte(rs_232_port_base + RS_232_INTERRUPT_IDENTIFICATION_REG, 0xC7);
    writeByte(rs_232_port_base + RS_232_MODEM_CONTROL_REG, 0x0B);
    writeByte(rs_232_port_base + RS_232_LINE_CONTROL_REG, 0x00);
    // re-enable interrupts
    writeByte(rs_232_port_base + RS_232_INTERRUPT_ENABLE_REG, 0x01);
    // Print out header info to the serial
    rs232_print(
        "\033[93m\n    ____              _              _____\n"
        "   / __ \\____ _____  (_)  __   _   _|__  /\n"
        "  / /_/ / __ `/ __ \\/ / |/_/  | | / //_ < \n"
        " / ____/ /_/ / / / / />  <    | |/ /__/ / \n"
        "/_/    \\__,_/_/ /_/_/_/|_|    |___/____/  \n\n\033[0m"
        "Panix Serial Output Debugger\n\n"
    );
}

char rs232_get_char() {
    mutex_lock(&mutex_rs232);
    // Grab the last byte and convert to a char
    uint8_t data = 0;
    ring.Dequeue(&data);
    mutex_unlock(&mutex_rs232);
    return (char)data;
}

int rs232_get_str(char* str, int max) {
    mutex_lock(&mutex_rs232);
    int idx = 0;
    // Keep reading until the buffer is empty or
    // a newline is read.
    while (!ring.IsEmpty()) {
        uint8_t byte = 0;
        ring.Dequeue(&byte);
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
    mutex_unlock(&mutex_rs232);
    // Return the string
    return idx;
}

int rs232_close() {
    // Clear data buffer in case reopened.
    memset(rs_232_data, 0, sizeof(rs_232_data));
    return 0;
}
