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

static uint16_t _ansi_values[8] = { 0 };
static size_t _ansi_values_index = 0;
#define PUSH_VAL(VAL) _ansi_values[_ansi_values_index++] = (VAL)
#define POP_VAL() _ansi_values[--_ansi_values_index]
#define CLEAR_VALS() _ansi_values_index = 0
#define ESC ('\033')
// ANSI states
typedef enum ansi_state {
    Normal,
    Esc,
    Bracket,
    Value
} ansi_state_t;
// State and value storage
ansi_state_t _ansi_state = Normal;
uint16_t _ansi_val = 0;
// Saved cursor positions
uint8_t _ansi_cursor_x = 0;
uint8_t _ansi_cursor_y = 0;
// The names don't really line up, so this will need refactoring.
uint16_t _px_ansi_vga_table[16] = {
    VGA_Black, VGA_Red, VGA_Green, VGA_Brown, VGA_Blue,
    VGA_Magenta, VGA_Cyan, VGA_LightGrey, VGA_DarkGrey, VGA_LightRed,
    VGA_LightGreen, VGA_Yellow, VGA_LightBlue, VGA_LightMagenta,
    VGA_LightCyan, VGA_White
};
// Printing mutual exclusion
px_mutex_t put_mutex;

int putchar(char c)
{
    int retval;
    // must lock when writing to the screen
    px_mutex_lock(&put_mutex);
    // call the unlocked implementation of putchar
    retval = putchar_unlocked(c);
    // release the screen to be used by other tasks
    px_mutex_unlock(&put_mutex);
    return retval;
}

int putchar_unlocked(char c) {
    // Moved to avoid cross initialization when calling goto error
    volatile uint16_t* where;
    uint16_t attrib = (color_back << 4) | (color_fore & 0x0F);
    // Check the ANSI state
    switch (_ansi_state) {
        case Normal: // print the character out normally unless it's an ESC
            if (c != ESC) break;
            _ansi_state = Esc;
            goto end;
        case Esc: // we got an ESC, now we need a left square bracket
            if (c != '[') break;
            _ansi_state = Bracket;
            goto end;
        case Bracket: // we're looking for a value/command char now
            if (c >= '0' && c <= '9') {
                _ansi_val = (uint16_t)(c - '0');
                _ansi_state = Value;
                goto end;
            }
            else if (c == 's') { // Save cursor position attribute
                _ansi_cursor_x = tty_coords_x;
                _ansi_cursor_y = tty_coords_y;
                goto normal;
            } 
            else if (c == 'u') { // Restore cursor position attribute
                tty_coords_x = _ansi_cursor_x;
                tty_coords_y = _ansi_cursor_y;
                goto normal;
            }
            break;
        case Value:
            if (c == ';') { // the semicolon is a value separator
                // enqueue the value here
                PUSH_VAL(_ansi_val);
                _ansi_state = Bracket;
                _ansi_val = 0;
            } else if (c == 'm') { // Set color/text attributes command
                PUSH_VAL(_ansi_val);
                // take action here
                // iterate through all values
                while (_ansi_values_index > 0) {
                    _ansi_val = POP_VAL();
                    if (_ansi_val == 0) {
                        // Reset code will just reset to whatever was specified in tty_clear().
                        color_fore = reset_fore;
                        color_back = reset_back;
                    } else if (_ansi_val >= ANSI_Black && _ansi_val <= ANSI_White) {
                        color_fore = (px_tty_vga_color)_px_ansi_vga_table[_ansi_val - ANSI_Black];
                    } else if (_ansi_val >= (ANSI_Black + 10) && _ansi_val <= (ANSI_White + 10)) {
                        color_back = (px_tty_vga_color)_px_ansi_vga_table[_ansi_val - (ANSI_Black + 10)];
                    } else if (_ansi_val >= ANSI_BrightBlack && _ansi_val <= ANSI_BrightWhite) {
                        color_fore = (px_tty_vga_color)_px_ansi_vga_table[_ansi_val - ANSI_BrightBlack + 8];
                    } else if (_ansi_val >= (ANSI_BrightBlack + 10) && _ansi_val <= (ANSI_BrightWhite + 10)) {
                        color_back = (px_tty_vga_color)_px_ansi_vga_table[_ansi_val - (ANSI_BrightBlack + 10) + 8];
                    } // else it was an unknown code
                }
                goto normal;
            } else if (c == 'H' || c == 'f') { // Set cursor position attribute
                PUSH_VAL(_ansi_val);
                // the proper order is 'line (y);column (x)'
                if (_ansi_values_index > 2) {
                    goto error;
                }
                tty_coords_x = POP_VAL();
                tty_coords_y = POP_VAL();
                goto normal;
            }
            else if (c == 'J') { // Clear screen attribute
                // The proper code is ESC[2J
                if (_ansi_val != 2) {
                    goto error;
                }
                px_tty_clear();
            } else if (c >= '0' && c <= '9') { // just another digit of a value
                _ansi_val = _ansi_val * 10 + (uint16_t)(c - '0');
            } else break; // invald code, so just return to normal
            // we hit one of the cases so return
            goto end;
    }
    // we fell through some way or another so just reset to Normal no matter what
    _ansi_state = Normal;
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
        // Carriage return
        case '\r':
            tty_coords_x = 0;
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
    goto end;
error:
    // Reset stack index
    CLEAR_VALS();
    // Return to normal
    _ansi_state = Normal;
    _ansi_val = 0;
    return EOF;
normal:
    _ansi_state = Normal;
    _ansi_val = 0;
end:
    return (int)c;
}
