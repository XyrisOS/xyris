/**
 * @file kprint.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief px_kprint is a small library to print unformatted strings to
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
#include <devices/tty/tty.hpp>
#include <lib/stdio.hpp>

uint8_t tty_coords_x = 0;
uint8_t tty_coords_y = 0;
px_tty_color color_back = Black;
px_tty_color color_fore = White;

void px_print_debug(char* msg, px_print_level lvl) {
    // Reset the color to the default and print the opening bracket
    px_tty_set_color(White, Black);
    px_kprintf("[");
    // Change the color and print the tag according to the level
    switch (lvl) {
        case Info:
            px_tty_set_color(LightGrey, Black);
            px_kprint(" INFO ");
            break;
        case Warning:
            px_tty_set_color(Yellow, Black);
            px_kprint(" WARN ");
            break;
        case Error:
            px_tty_set_color(Red, Black);
            px_kprint(" FAIL ");
            break;
        case Success:
            px_tty_set_color(LightGreen, Black);
            px_kprint("  OK  ");
            break;
        default:
            px_tty_set_color(Magenta, Black);
            px_kprint(" ???? ");
            break;
    }
    // Reset the color to the default and print the closing bracket and message
    px_tty_set_color(White, Black);
    px_kprintf("] %s\n", msg);
}

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

void px_kprint(const char* str) {
    px_kprintf(str);
}

void px_kprint_hex(uint32_t key) {
    px_kprintf("0x%08X", key);
}

void px_kprint_color(char* str, px_tty_color color) {
    px_tty_color oldBack = color_back;
    px_tty_color oldFore = color_fore;
    px_tty_set_color(color, color_back);
    px_kprintf(str);
    px_tty_set_color(oldFore, oldBack);
}

void px_tty_set_color(px_tty_color fore, px_tty_color back) {
    color_fore = fore;
    color_back = back;
}

void px_clear_tty() {
    tty_coords_x = 0;
    tty_coords_y = 0;
    char c = ' ';
    for (int y = 0; y < X86_TTY_HEIGHT; y++) {
        for (int x = 0; x < X86_TTY_WIDTH; x++) {
            putchar(c);
        }
    }
    // Reset the cursor position
    tty_coords_x = 0;
    tty_coords_y = 0;
}

void px_set_indicator(px_tty_color color) {
    volatile uint16_t* where;
    uint16_t attrib = (color << 4) | (color & 0x0F);
    where = x86_bios_vga_mem + (X86_IND_Y * X86_TTY_WIDTH + X86_IND_X);
    *where = ' ' | (attrib << 8);
}

void putchar(char c) {
    volatile uint16_t* where;
    uint16_t attrib = (color_back << 4) | (color_fore & 0x0F);
    switch(c) {
        // Backspace
        case 0x08:
            if (tty_coords_x > 0) {
                tty_coords_x--;
            }
            where = x86_bios_vga_mem + (tty_coords_y * X86_TTY_WIDTH + tty_coords_x);
            *where = ' ' | (attrib << 8);
            break;
        // Newline
        case '\n':
            tty_coords_x = 0;
            tty_coords_y++;
            break;
        // Anything else
        default:
            where = x86_bios_vga_mem + (tty_coords_y * X86_TTY_WIDTH + tty_coords_x);
            *where = c | (attrib << 8);
            tty_coords_x++;
            break;
    }
    // Move to the next line
    if(tty_coords_x >= X86_TTY_WIDTH) {
        tty_coords_x = 0;
        tty_coords_y++;
    }
    // Clear the screen
    if(tty_coords_y >= X86_TTY_HEIGHT) {
        px_shift_tty_up();
        where = x86_bios_vga_mem + ((X86_TTY_HEIGHT - 1) * X86_TTY_WIDTH - 1);
        for (size_t col = 0; col < X86_TTY_WIDTH; ++col) {
            *(++where) = ' ' | (attrib << 8);
        }
        tty_coords_x = 0;
        tty_coords_y = X86_TTY_HEIGHT - 1;
    }
}
