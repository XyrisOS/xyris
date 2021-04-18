/**
 * @file tty.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief TTY is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * px_kprintf().
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#include <dev/tty/tty.hpp>
#include <lib/stdio.hpp>
#include <stddef.h>

// Coorinate trackers
uint8_t tty_coords_x = 0;
uint8_t tty_coords_y = 0;
// VGA colors (defaults are white on black)
px_tty_vga_color color_back = VGA_Black;
px_tty_vga_color color_fore = VGA_White;
// Default colors set by tty_clear()
px_tty_vga_color reset_back = VGA_Black;
px_tty_vga_color reset_fore = VGA_White;

void px_shift_tty_up() {
    // start on the second row
    volatile uint16_t* where = x86_bios_vga_mem + X86_TTY_WIDTH;
    for (size_t row = 1; row < X86_TTY_HEIGHT; ++row) {
        for (size_t col = 0; col < X86_TTY_WIDTH; ++col) {
            // copy the char to the previous row
            *(where - X86_TTY_WIDTH) = *where;
            // increment the pointer
            ++where;
        }
    }
}

void px_tty_clear(px_tty_vga_color fore, px_tty_vga_color back) {
    color_fore = fore;
    color_back = back;
    reset_fore = fore;
    reset_back = back;
    volatile uint16_t* where;
    uint16_t attrib = (uint16_t)VGA_COLOR(color_back, color_fore);
    // For each character in each line of the TTY, set to ' '.
    for (int y = 0; y < X86_TTY_HEIGHT; y++) {
        for (int x = 0; x < X86_TTY_WIDTH; x++) {
            // This is a direct write to the BIOS TTY memory.
            // It is much more efficient than calling putchar().
            where = x86_bios_vga_mem + (y * X86_TTY_WIDTH + x);
            *where = (uint16_t)VGA_CHAR(' ', attrib);
        }
    }
    // Reset the cursor position
    tty_coords_x = 0;
    tty_coords_y = 0;
}

void px_tty_reset_defaults() {
    color_back = VGA_DEFAULT_BACK;
    color_fore = VGA_DEFAULT_FORE;
}

void px_set_indicator(px_tty_vga_color color) {
    volatile uint16_t* where;
    uint16_t attrib = (uint16_t)((color << 4) | (color & 0x0F));
    where = x86_bios_vga_mem + (X86_IND_Y * X86_TTY_WIDTH + X86_IND_X);
    *where = VGA_CHAR(' ', attrib);
}
