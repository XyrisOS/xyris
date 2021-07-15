/**
 * @file putchar.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-08
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

#include <stddef.h>
#include <lib/stdio.hpp>
#include <lib/mutex.hpp>
#include <dev/tty/tty.hpp>

static uint16_t ansi_values[8] = { 0 };
static size_t ansi_values_index = 0;
#define PUSH_VAL(VAL) ansi_values[ansi_values_index++] = (VAL)
#define POP_VAL() ansi_values[--ansi_values_index]
#define CLEAR_VALS() ansi_values_index = 0
#define ESC ('\033')
#define TAB_WIDTH 4u
// ANSI states
typedef enum ansi_state {
    Normal,
    Esc,
    Bracket,
    Value
} ansi_state_t;
// State and value storage
ansi_state_t ansi_state = Normal;
uint16_t ansi_val = 0;
// Saved cursor positions
uint8_t ansi_cursor_x = 0;
uint8_t ansi_cursor_y = 0;
// The names don't really line up, so this will need refactoring.
uint16_t ansi_vga_table[16] = {
    VGA_Black, VGA_Red, VGA_Green, VGA_Brown, VGA_Blue,
    VGA_Magenta, VGA_Cyan, VGA_LightGrey, VGA_DarkGrey, VGA_LightRed,
    VGA_LightGreen, VGA_Yellow, VGA_LightBlue, VGA_LightMagenta,
    VGA_LightCyan, VGA_White
};
// Printing mutual exclusion
Mutex put_mutex;

int putchar(char c)
{
    int retval;
    // must lock when writing to the screen
    put_mutex.Lock();
    // call the unlocked implementation of putchar
    retval = putchar_unlocked(c);
    // release the screen to be used by other tasks
    put_mutex.Unlock();
    return retval;
}

int putchar_unlocked(char c) {
    // Moved to avoid cross initialization when calling goto error
    volatile uint16_t* where;
    uint16_t attrib = (uint16_t)((color_back << 4) | (color_fore & 0x0F));
    // Check the ANSI state
    switch (ansi_state) {
        case Normal: // print the character out normally unless it's an ESC
            if (c != ESC) break;
            ansi_state = Esc;
            goto end;
        case Esc: // we got an ESC, now we need a left square bracket
            if (c != '[') break;
            ansi_state = Bracket;
            goto end;
        case Bracket: // we're looking for a value/command char now
            if (c >= '0' && c <= '9') {
                ansi_val = (uint16_t)(c - '0');
                ansi_state = Value;
                goto end;
            }
            else if (c == 's') { // Save cursor position attribute
                ansi_cursor_x = tty_coords_x;
                ansi_cursor_y = tty_coords_y;
                goto normal;
            }
            else if (c == 'u') { // Restore cursor position attribute
                tty_coords_x = ansi_cursor_x;
                tty_coords_y = ansi_cursor_y;
                goto normal;
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
                    ansi_val = (uint16_t)POP_VAL();
                    if (ansi_val == 0) {
                        // Reset code will just reset to whatever was specified in tty_clear().
                        color_fore = reset_fore;
                        color_back = reset_back;
                    } else if (ansi_val >= ANSI_Black && ansi_val <= ANSI_White) {
                        color_fore = (tty_vga_color)ansi_vga_table[ansi_val - ANSI_Black];
                    } else if (ansi_val >= (ANSI_Black + 10) && ansi_val <= (ANSI_White + 10)) {
                        color_back = (tty_vga_color)ansi_vga_table[ansi_val - (ANSI_Black + 10)];
                    } else if (ansi_val >= ANSI_BrightBlack && ansi_val <= ANSI_BrightWhite) {
                        color_fore = (tty_vga_color)ansi_vga_table[ansi_val - ANSI_BrightBlack + 8];
                    } else if (ansi_val >= (ANSI_BrightBlack + 10) && ansi_val <= (ANSI_BrightWhite + 10)) {
                        color_back = (tty_vga_color)ansi_vga_table[ansi_val - (ANSI_BrightBlack + 10) + 8];
                    } // else it was an unknown code
                }
                goto normal;
            } else if (c == 'H' || c == 'f') { // Set cursor position attribute
                PUSH_VAL(ansi_val);
                // the proper order is 'line (y);column (x)'
                if (ansi_values_index > 2) {
                    goto error;
                }
                tty_coords_x = (uint8_t)POP_VAL();
                tty_coords_y = (uint8_t)POP_VAL();
                goto normal;
            }
            else if (c == 'J') { // Clear screen attribute
                // The proper code is ESC[2J
                if (ansi_val != 2) {
                    goto error;
                }
                tty_clear();
            } else if (c >= '0' && c <= '9') { // just another digit of a value
                ansi_val = (uint16_t)(ansi_val * 10 + (uint16_t)(c - '0'));
            } else break; // invald code, so just return to normal
            // we hit one of the cases so return
            goto end;
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
            *where = (uint16_t)(' ' | (attrib << 8));
            break;
        // Newline
        case '\n':
            tty_coords_x = 0;
            tty_coords_y++;
            break;
        case '\t':
            while (++tty_coords_x % TAB_WIDTH) {
                where = x86_bios_vga_mem + (tty_coords_y * X86_TTY_WIDTH + tty_coords_x);
                *where = (uint16_t)(' ' | (attrib << 8));
            }
            break;
        // Carriage return
        case '\r':
            tty_coords_x = 0;
            break;
        // Anything else
        default:
            where = x86_bios_vga_mem + (tty_coords_y * X86_TTY_WIDTH + tty_coords_x);
            *where = (uint16_t)(c | (attrib << 8));
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
        shift_tty_up();
        where = x86_bios_vga_mem + ((X86_TTY_HEIGHT - 1) * X86_TTY_WIDTH - 1);
        for (size_t col = 0; col < X86_TTY_WIDTH; ++col) {
            *(++where) = (uint16_t)(' ' | (attrib << 8));
        }
        tty_coords_x = 0;
        tty_coords_y = X86_TTY_HEIGHT - 1;
    }
    goto end;
error:
    // Reset stack index
    CLEAR_VALS();
    // Return to normal
    ansi_state = Normal;
    ansi_val = 0;
    return EOF;
normal:
    ansi_state = Normal;
    ansi_val = 0;
end:
    return (int)c;
}
