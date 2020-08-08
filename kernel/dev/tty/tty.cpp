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
#include <dev/tty/tty.hpp>
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
uint8_t ansi_cursor_x = 0;
uint8_t ansi_cursor_y = 0;
px_tty_vga_color color_back = VGA_Black;
px_tty_vga_color color_fore = VGA_White;
// Colors set by tty_clear()
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
    uint16_t attrib = (color_back << 4) | (color_fore & 0x0F);
    // For each character in each line of the TTY, set to ' '.
    for (int y = 0; y < X86_TTY_HEIGHT; y++) {
        for (int x = 0; x < X86_TTY_WIDTH; x++) {
            // This is a direct write to the BIOS TTY memory.
            // It is much more efficient than calling putchar().
            where = x86_bios_vga_mem + (y * X86_TTY_WIDTH + x);
            *where = ' ' | (attrib << 8);
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
    uint16_t attrib = (color << 4) | (color & 0x0F);
    where = x86_bios_vga_mem + (X86_IND_Y * X86_TTY_WIDTH + X86_IND_X);
    *where = ' ' | (attrib << 8);
}

static uint16_t ansi_values[8] = { 0 };
static size_t ansi_values_index = 0;
#define PUSH_VAL(VAL) ansi_values[ansi_values_index++] = (VAL)
#define POP_VAL() ansi_values[--ansi_values_index]
#define CLEAR_VALS() ansi_values_index = 0

int putchar(char c) {
    // Moved to avoid cross initialization when calling goto error
    volatile uint16_t* where;
    uint16_t attrib = (color_back << 4) | (color_fore & 0x0F);
    // Check the ANSI state
    switch (ansi_state) {
        case Normal: // print the character out normally unless it's an ESC
            if (c != ESC) break;
            ansi_state = Esc;
            return c;
        case Esc: // we got an ESC, now we need a left square bracket
            if (c != '[') break;
            ansi_state = Bracket;
            return c;
        case Bracket: // we're looking for a value/command char now
            if (c >= '0' && c <= '9') {
                ansi_val = (uint16_t)(c - '0');
                ansi_state = Value;
                return c;
            }
            else if (c == 's') { // Save cursor position attribute
                ansi_cursor_x = tty_coords_x;
                ansi_cursor_y = tty_coords_y;
                return c;
            } 
            else if (c == 'u') { // Restore cursor position attribute
                tty_coords_x = ansi_cursor_x;
                tty_coords_y = ansi_cursor_y;
                return c;
            }
            break;
        case Value:
            if (c == ';') { // the semicolon is a value separator
                // enqueue the value here
                PUSH_VAL(ansi_val);
                ansi_state = Bracket;
                ansi_val = 0;
            } else if (c == 'm') { // Set color/text attributes command
                PUSH_VAL(ansi_val);
                // take action here
                // iterate through all values
                while (ansi_values_index > 0) {
                    ansi_val = POP_VAL();
                    if (ansi_val == 0) {
                        // Reset code will just reset to whatever was specified in tty_clear().
                        color_fore = reset_fore;
                        color_back = reset_back;
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
            } else if (c == 'H' || c == 'f') { // Set cursor position attribute
                PUSH_VAL(ansi_val);
                // the proper order is 'line (y);column (x)'
                if (ansi_values_index > 2) {
                    goto error;
                }
                tty_coords_x = POP_VAL();
                tty_coords_y = POP_VAL();
            }
            else if (c == 'J') { // Clear screen attribute
                // The proper code is ESC[2J
                if (ansi_val == 2) {
                    px_tty_clear();
                }
            } else if (c >= '0' && c <= '9') { // just another digit of a value
                ansi_val = ansi_val * 10 + (uint16_t)(c - '0');
            } else break; // invald code, so just return to normal
            // we hit one of the cases so return
            return c;
    }
    // we fell through some way or another so just reset to Normal no matter what
    ansi_state = Normal;
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
    return c;
error:
    // Reset stack index
    CLEAR_VALS();
    // Return to normal
    ansi_state = Normal;
    ansi_val = 0;
    return EOF;
}

int puts(const char *str) {
    int i = 0;
    // Loops until a null character
    while(str[i]) {
        if(putchar(str[i]) == EOF) { 
            return EOF;
        }
        i++;
    }
    if(putchar('\n') == EOF) {
       return EOF;
    }
    // Follow POSIX spec
    return 1;
}
