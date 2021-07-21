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
#include <lib/RingBuffer.hpp>

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

namespace rs232 {

static uint16_t rs_232_port_base;
static RingBuffer<char, 1024> ring;
static Mutex mutex_rs232("rs232");

static int received();
static int is_transmit_empty();
static char read_byte();
static void callback(registers_t *regs);

static int received() {
    return readByte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 1;
}

static int is_transmit_empty() {
    return readByte(rs_232_port_base + RS_232_LINE_STATUS_REG) & 0x20;
}

static char read_byte() {
    mutex_rs232.Lock();
    while (received() == 0);
    mutex_rs232.Unlock();
    return readByte(rs_232_port_base + RS_232_DATA_REG);
}

static int vprintf_helper(unsigned c, void **ptr)
{
    // Unfortunately very hacky...
    (void)ptr;
    char buf = (char)c;
    write(&buf, 1);
    return 0;
}

int vprintf(const char* fmt, va_list args)
{
    int retval;
    retval = do_printf(fmt, args, vprintf_helper, NULL);
    return retval;
}

int printf(const char *format, ...)
{
    va_list args;
    int ret_val;

    va_start(args, format);
    ret_val = vprintf(format, args);
    va_end(args);
    return ret_val;
}

static void callback(registers_t *regs) {
    (void)regs;
    // Grab the input character
    char in = read_byte();
    // Change carriage returns to newlines
    if (in == '\r') {
        in = '\n';
    }
    // Create a string and print it so that the
    // user can see what they're typing.
    char str[2] = {in, '\0'};
    printf("%s", str);
    // Add the character to the circular buffer
    ring.Enqueue(in);
}

// FIXME: Use separate ring buffers for COM1 & COM2
void init(uint16_t com_id) {
    // Register the IRQ callback
    rs_232_port_base = com_id;
    uint8_t IRQ = 0x20 + (com_id == RS_232_COM1 ? RS_232_COM1_IRQ : RS_232_COM2_IRQ);
    register_interrupt_handler(IRQ, callback);
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
    printf("%s",
        "\033[93m\n    ____              _              _____\n"
        "   / __ \\____ _____  (_)  __   _   _|__  /\n"
        "  / /_/ / __ `/ __ \\/ / |/_/  | | / //_ < \n"
        " / ____/ /_/ / / / / />  <    | |/ /__/ / \n"
        "/_/    \\__,_/_/ /_/_/_/|_|    |___/____/  \n\n\033[0m"
        "Panix Serial Output Debugger\n\n"
    );
}

size_t read(char* buf, size_t count) {
    size_t bytes = 0;
    mutex_rs232.Lock();
    for (size_t idx = 0; idx < count && !ring.IsEmpty(); idx++)
    {
        buf[idx] = ring.Dequeue();
        bytes++;
    }
    mutex_rs232.Unlock();
    return bytes;
}

size_t write(const char* buf, size_t count) {
    // Wait for previous transfer to complete
    while (is_transmit_empty() == 0);
    size_t bytes = 0;
    for (size_t idx = 0; idx < count; idx++) {
        writeByte(rs_232_port_base + RS_232_DATA_REG, buf[idx]);
        bytes++;
    }
    return bytes;
}

int close() {
    return 0;
}

};
