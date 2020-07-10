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

uint8_t ttyCoordsX = 0;
uint8_t ttyCoordsY = 0;
px_tty_color backColor = Black;
px_tty_color foreColor = White;

void px_print_debug(char* msg, px_print_level lvl) {
    // Reset the color to the default and print the opening bracket
    px_tty_set_color(White, Black);
    px_kprint("[");
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
    px_kprint("] ");
    px_kprint(msg);
    px_kprint("\n");
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
    // For each character in the string
    for(int i = 0; str[i] != '\0'; ++i) {
        putchar(str[i]);
    }
}

void px_kprint_hex(uint32_t key) {
    char* foo = "00000000";
    static const char* hex = "0123456789ABCDEF";
    for (size_t i = 0; i < 8; ++i) {
        foo[7 - i] = hex[(key >> (i * 4)) & 0x0f];
    }
    px_kprint(foo);
}

void px_kprint_color(char* str, px_tty_color color) {
    px_tty_color oldBack = backColor;
    px_tty_color oldFore = foreColor;
    px_tty_set_color(color, backColor);
    px_kprint(str);
    px_tty_set_color(oldFore, oldBack);
}

void px_tty_set_color(px_tty_color fore, px_tty_color back) {
    foreColor = fore;
    backColor = back;
}

void px_clear_tty() {
    ttyCoordsX = 0;
    ttyCoordsY = 0;
    char c = ' ';
    for (int y = 0; y < X86_TTY_HEIGHT; y++) {
        for (int x = 0; x < X86_TTY_WIDTH; x++) {
            putchar(c);
        }
    }
    // Reset the cursor position
    ttyCoordsX = 0;
    ttyCoordsY = 0;
}

void px_set_indicator(px_tty_color color) {
    volatile uint16_t* where;
    uint16_t attrib = (color << 4) | (color & 0x0F);
    where = x86_bios_vga_mem + (X86_IND_Y * X86_TTY_WIDTH + X86_IND_X);
    *where = ' ' | (attrib << 8);
}

void putchar(char c) {
    volatile uint16_t* where;
    uint16_t attrib = (backColor << 4) | (foreColor & 0x0F);
    switch(c) {
        // Backspace
        case 0x08:
            if (ttyCoordsX > 0) {
                ttyCoordsX--;
            }
            where = x86_bios_vga_mem + (ttyCoordsY * X86_TTY_WIDTH + ttyCoordsX);
            *where = ' ' | (attrib << 8);
            break;
        // Newline
        case '\n':
            ttyCoordsX = 0;
            ttyCoordsY++;
            break;
        // Anything else
        default:
            where = x86_bios_vga_mem + (ttyCoordsY * X86_TTY_WIDTH + ttyCoordsX);
            *where = c | (attrib << 8);
            ttyCoordsX++;
            break;
    }
    // Move to the next line
    if(ttyCoordsX >= X86_TTY_WIDTH) {
        ttyCoordsX = 0;
        ttyCoordsY++;
    }
    // Clear the screen
    if(ttyCoordsY >= X86_TTY_HEIGHT) {
        px_shift_tty_up();
        where = x86_bios_vga_mem + ((X86_TTY_HEIGHT - 1) * X86_TTY_WIDTH - 1);
        for (size_t col = 0; col < X86_TTY_WIDTH; ++col) {
            *(++where) = ' ' | (attrib << 8);
        }
        ttyCoordsX = 0;
        ttyCoordsY = X86_TTY_HEIGHT - 1;
    }
}
