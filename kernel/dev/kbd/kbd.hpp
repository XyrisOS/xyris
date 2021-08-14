/**
 * @file kbd.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-18
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once

#include <stdint.h>

#define KBD_COMMAND_PORT 0x64
#define KBD_DATA_PORT 0x60

// Keyboard scancode definitions
#define KBD_BACKSPACE   0x0E
#define KBD_ENTER       0x1C
#define KBD_LEFT_SHIFT  0x2A
#define KBD_RIGHT_SHIFT 0x36
#define KBD_ARROW_UP    0x101
#define KBD_ARROW_DOWN  0x102
#define KBD_ARROW_RIGHT 0x103
#define KBD_ARROW_LEFT  0x104

void kbd_init();

// A 256 int long buffer of our scancode inputs.
// Eventually this will need to be shared memory
// or piped into usermode.
extern uint8_t kbd_scode_buff[256];
// Position index to get the latest scancode.
// This index is circular so when we would run
// off the end of the array we loop back to 0.
extern uint8_t kbd_scode_buff_pos;
