/**
 * @file kbd.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2019-11-18
 * 
 * @copyright Copyright the Panix Contributors (c) 2019
 * 
 */

#include <arch/arch.hpp>
#include <devices/kbd/kbd.hpp>
#include <devices/kbd/scode.hpp>

uint8_t px_kbd_scode_buff[256];
uint8_t px_kbd_scode_buff_pos = 0;

void px_kbd_push_scode(uint8_t scode);

static void px_kbd_callback(registers_t *regs) {
    uint8_t scode = px_read_byte(KBD_DATA_PORT);
    // If the keyboard scancode corresponds to an "on key down"
    px_kbd_push_scode(scode);
}

void px_kbd_init() {
    px_register_interrupt_handler(IRQ1, px_kbd_callback);
}

void px_kbd_push_scode(uint8_t scode) {
    // Push our scancode onto the buffer
    px_kbd_scode_buff[px_kbd_scode_buff_pos] = scode;
    // Increment and loop back if necessary
    px_kbd_scode_buff_pos = (px_kbd_scode_buff_pos + 1) % 256;
}