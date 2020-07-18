/**
 * @file kprint.cpp
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
#include <devices/tty/tty.hpp>
#include <lib/stdio.hpp>
#include <stddef.h>

#define ESC ('\033')

enum tty_state {
    Normal,
    Esc,
    Bracket,
    Value
};

// The names don't really line up, so this will need refactoring.
uint16_t px_ansi_vga_table[16] = {
    VGA_Black, VGA_Red, VGA_Green, VGA_Brown, VGA_Blue,
    VGA_Magenta, VGA_Cyan, VGA_LightGrey, VGA_DarkGrey, VGA_LightRed,
    VGA_LightGreen, VGA_Yellow, VGA_LightBlue, VGA_LightMagenta,
    VGA_LightCyan, VGA_White
};

tty_state ansi_state = Normal;
uint16_t ansi_val = 0;

uint8_t tty_coords_x = 0;
uint8_t tty_coords_y = 0;
px_tty_vga_color color_back = VGA_Black;
px_tty_vga_color color_fore = VGA_White;

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

void px_clear_tty(px_tty_vga_color fore, px_tty_vga_color back) {
    color_fore = fore;
    color_back = back;
    tty_coords_x = 0;
    tty_coords_y = 0;
    // TODO: Do something more efficient than putchar().
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

void px_set_indicator(px_tty_vga_color color) {
    volatile uint16_t* where;
    uint16_t attrib = (color << 4) | (color & 0x0F);
    where = x86_bios_vga_mem + (X86_IND_Y * X86_TTY_WIDTH + X86_IND_X);
    *where = ' ' | (attrib << 8);
}

static uint16_t ansi_values[8] = { 0 };
static size_t ansi_values_index = 0;
#define PUSH_VAL(VAL) ansi_values[ansi_values_index++] = (VAL)
#define POP_VAL() ansi_values[--ansi_values_index]

void putchar(char c) {
    switch (ansi_state) {
        case Normal: // print the character out normally unless it's an ESC
            if (c != ESC) break;
            ansi_state = Esc;
            return;
        case Esc: // we got an ESC, now we need a left square bracket
            if (c != '[') break;
            ansi_state = Bracket;
            return;
        case Bracket: // we're looking for a value/command char now
            if (c >= '0' && c <= '9') {
                ansi_val = (uint16_t)(c - '0');
                ansi_state = Value;
                return;
            }
            // handle any other control codes here
            break;
        case Value:
            if (c == ';') { // the semicolon is a value separator
                // enqueue the value here
                PUSH_VAL(ansi_val);
                ansi_state = Bracket;
                ansi_val = 0;
            } else if (c == 'm') { // the color/text attributes command
                PUSH_VAL(ansi_val);
                // take action here
                // iterate through all values
                while (ansi_values_index > 0) {
                    ansi_val = POP_VAL();
                    if (ansi_val == 0) {
                        color_fore = VGA_DEFAULT_FORE;
                        color_back = VGA_DEFAULT_BACK;
                    } else if (ansi_val >= ANSI_Black && ansi_val <= ANSI_White) {
                        color_fore = (px_tty_vga_color)px_ansi_vga_table[ansi_val - ANSI_Black];
                    } else if (ansi_val >= (ANSI_Black + 10) && ansi_val <= (ANSI_White + 10)) {
                        color_back = (px_tty_vga_color)px_ansi_vga_table[ansi_val - (ANSI_Black + 10)];
                    } else if (ansi_val >= ANSI_BrightBlack && ansi_val <= ANSI_BrightWhite) {
                        color_fore = (px_tty_vga_color)px_ansi_vga_table[ansi_val - ANSI_BrightBlack + 8];
                    } else if (ansi_val >= (ANSI_BrightBlack + 10) && ansi_val <= (ANSI_BrightWhite + 10)) {
                        color_back = (px_tty_vga_color)px_ansi_vga_table[ansi_val - (ANSI_BrightBlack + 10) + 8];
                    } // else it was an unknown code
                }
                ansi_state = Normal;
                ansi_val = 0;
            } else if (c >= '0' && c <= '9') { // just another digit of a value
                ansi_val = ansi_val * 10 + (uint16_t)(c - '0');
            } else break; // invald code, so just return to normal
            // we hit one of the cases so return
            return;
    }
    // we fell through some way or another so just reset to Normal no matter what
    ansi_state = Normal;
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
