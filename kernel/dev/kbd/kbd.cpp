/**
 * @file kbd.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-18
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */

#include <arch/arch.hpp>
#include <dev/kbd/kbd.hpp>
#include <dev/kbd/scode.hpp>

uint8_t kbd_scode_buff[256];
uint8_t kbd_scode_buff_pos = 0;

void kbd_push_scode(uint8_t scode);

static void kbd_callback(registers_t *regs) {
    // Ignore the registers for now
    (void)regs;
    uint8_t scode = readByte(KBD_DATA_PORT);
    // If the keyboard scancode corresponds to an "on key down"
    kbd_push_scode(scode);
}

void kbd_init() {
    register_interrupt_handler(IRQ1, kbd_callback);
}

void kbd_push_scode(uint8_t scode) {
    // Push our scancode onto the buffer
    kbd_scode_buff[kbd_scode_buff_pos] = scode;
    // Increment and loop back if necessary
    kbd_scode_buff_pos = (uint8_t)((kbd_scode_buff_pos + 1) % 256);
}