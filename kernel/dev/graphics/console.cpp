/**
 * @file tty.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Framebuffer console
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#include <dev/graphics/console.hpp>
#include <dev/graphics/framebuffer.hpp>
#include <dev/graphics/graphics.hpp>
#include <dev/graphics/font.hpp>
#include <lib/stdio.hpp>
#include <stddef.h>
#include <stdarg.h>

namespace Console {

enum vgaColor : uint32_t {
    VGA_Black           = 0x000000,
    VGA_Blue            = 0x0000AA,
    VGA_Green           = 0x00AA00,
    VGA_Cyan            = 0x00AAAA,
    VGA_Red             = 0xAA0000,
    VGA_Magenta         = 0xAA00AA,
    VGA_Brown           = 0xAA5500,
    VGA_LightGrey       = 0xAAAAAA,
    VGA_DarkGrey        = 0x555555,
    VGA_LightBlue       = 0x5555FF,
    VGA_LightGreen      = 0x55FF55,
    VGA_LightCyan       = 0x55FFFF,
    VGA_LightRed        = 0xFF5555,
    VGA_LightMagenta    = 0xFF55FF,
    VGA_Yellow          = 0xFFFF55,
    VGA_White           = 0xFFFFFF,
};

// Coorinate trackers
uint8_t ttyCoordsX = 0;
uint8_t ttyCoordsY = 0;
// VGA colors (defaults are white on black)
vgaColor colorBack = VGA_Black;
vgaColor colorFore = VGA_White;
// Default colors set by tty_clear()
vgaColor resetBack = VGA_Black;
vgaColor resetFore = VGA_White;

/**
 * @brief ANSI color codes for use in functions
 * like printf(). To change the color from the
 * foreground to the background, add 10 to the
 * desired color value.
 * (i.e. Red == 31 (fore)--> 41 (back))
 *
 */
enum ansiColor : uint16_t {
    Background = 10,
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    BrightBlack = 90,
    BrightRed = 91,
    BrightGreen = 92,
    BrightYellow = 93,
    BrightBlue = 94,
    BrightMagenta = 95,
    BrightCyan = 96,
    BrightWhite = 97,
};

static uint16_t ansiValues[8] = { 0 };
static size_t ansiValuesIdx = 0;
#define PUSH_VAL(VAL) ansiValues[ansiValuesIdx++] = (VAL)
#define POP_VAL() ansiValues[--ansiValuesIdx]
#define CLEAR_VALS() ansiValuesIdx = 0
#define ESC ('\033')
#define TAB_WIDTH 4u
// ANSI states
enum ansiStateType {
    Normal,
    Esc,
    Bracket,
    Value
};
// State and value storage
ansiStateType ansiState = Normal;
uint16_t ansiVal = 0;
// Saved cursor positions
uint32_t ansiCursorX = 0;
uint32_t ansiCursorY = 0;
// The names don't really line up, so this will need refactoring.
uint32_t ansiVGATable[16] = {
    VGA_Black, VGA_Red, VGA_Green, VGA_Brown, VGA_Blue,
    VGA_Magenta, VGA_Cyan, VGA_LightGrey, VGA_DarkGrey, VGA_LightRed,
    VGA_LightGreen, VGA_Yellow, VGA_LightBlue, VGA_LightMagenta,
    VGA_LightCyan, VGA_White
};

// Printing mutual exclusion
Mutex ttyLock;

static void Lock() { ttyLock.Lock(); }
static void Unlock() { ttyLock.Unlock(); }

static int putchar(unsigned c, void **ptr)
{
    (void)ptr;
    // Check the ANSI state
    switch (ansiState) {
    case Normal: // print the character out normally unless it's an ESC
        if (c != ESC)
            break;
        ansiState = Esc;
        goto end;
    case Esc: // we got an ESC, now we need a left square bracket
        if (c != '[')
            break;
        ansiState = Bracket;
        goto end;
    case Bracket: // we're looking for a value/command char now
        if (c >= '0' && c <= '9') {
            ansiVal = (uint16_t)(c - '0');
            ansiState = Value;
            goto end;
        } else if (c == 's') { // Save cursor position attribute
            ansiCursorX = ttyCoordsX;
            ansiCursorY = ttyCoordsY;
            goto normal;
        } else if (c == 'u') { // Restore cursor position attribute
            ttyCoordsX = ansiCursorX;
            ttyCoordsY = ansiCursorY;
            goto normal;
        }
        break;
    case Value:
        if (c == ';') { // the semicolon is a value separator
            // enqueue the value here
            PUSH_VAL(ansiVal);
            ansiState = Bracket;
            ansiVal = 0;
        } else if (c == 'm') { // Set color/text attributes command
            PUSH_VAL(ansiVal);
            // take action here
            // iterate through all values
            while (ansiValuesIdx > 0) {
                ansiVal = (uint16_t)POP_VAL();
                if (ansiVal == 0) {
                    // Reset code will just reset to whatever was specified in tty_clear().
                    colorFore = resetFore;
                    colorBack = resetBack;
                } else if (ansiVal >= Black && ansiVal <= White) {
                    colorFore = (vgaColor)ansiVGATable[ansiVal - Black];
                } else if (ansiVal >= (Black + Background) && ansiVal <= (White + Background)) {
                    colorBack = (vgaColor)ansiVGATable[ansiVal - (Black + Background)];
                } else if (ansiVal >= BrightBlack && ansiVal <= BrightWhite) {
                    colorFore = (vgaColor)ansiVGATable[ansiVal - BrightBlack + 8];
                } else if (ansiVal >= (BrightBlack + Background) && ansiVal <= (BrightWhite + Background)) {
                    colorBack = (vgaColor)ansiVGATable[ansiVal - (BrightBlack + Background) + 8];
                } // else it was an unknown code
            }
            goto normal;
        } else if (c == 'H' || c == 'f') { // Set cursor position attribute
            PUSH_VAL(ansiVal);
            // the proper order is 'line (y);column (x)'
            if (ansiValuesIdx > 2) {
                goto error;
            }
            ttyCoordsX = (uint8_t)POP_VAL();
            ttyCoordsY = (uint8_t)POP_VAL();
            goto normal;
        } else if (c == 'J') { // Clear screen attribute
            // The proper code is ESC[2J
            if (ansiVal != 2) {
                goto error;
            }
            // Clear by resetting the double buffer and swapping.
            // TODO: Find a better way to do this?
            debugf("Clearing screen\n");
            graphics::resetDoubleBuffer();
            graphics::swap();
        } else if (c >= '0' && c <= '9') { // just another digit of a value
            ansiVal = (uint16_t)(ansiVal * 10 + (uint16_t)(c - '0'));
        } else
            break; // invald code, so just return to normal
        // we hit one of the cases so return
        goto end;
    }
    // we fell through some way or another so just reset to Normal no matter what
    ansiState = Normal;
    switch (c) {
    // Backspace
    case 0x08:
        if (ttyCoordsX > 0) {
            ttyCoordsX--;
        }
        break;
    // Newline
    case '\n':
        ttyCoordsX = 0;
        ttyCoordsY++;
        break;
    case '\t':
        while (++ttyCoordsX % TAB_WIDTH);
        break;
    // Carriage return
    case '\r':
        ttyCoordsX = 0;
        break;
    }

    // Print the character
    graphics::font::Draw(c, ttyCoordsX++, ttyCoordsY, colorFore, colorBack);

    // Move to the next line
    if (ttyCoordsX >= X86_TTY_WIDTH) {
        debugf("Move to the next line\n");
        ttyCoordsX = 0;
        ttyCoordsY++;
    }
    // Clear the screen
    if (ttyCoordsY >= X86_TTY_HEIGHT) {
        debugf("Shift up the screen\n");
        //TODO: Shift text up and reset the bottom line
        ttyCoordsX = 0;
        ttyCoordsY = X86_TTY_HEIGHT - 1;
    }
    goto end;
error:
    // Reset stack index
    CLEAR_VALS();
    // Return to normal
    ansiState = Normal;
    ansiVal = 0;
    return -1;
normal:
    ansiState = Normal;
    ansiVal = 0;
end:
    return 0;
}

static int vprintf(const char* fmt, va_list args)
{
    int retval;
    Lock();
    retval = printf_helper(fmt, args, putchar, NULL);
    Unlock();
    return retval;
}

void write(const char c)
{
    Lock();
    putchar(c, NULL);
    Unlock();
}

void write(const char* str)
{
    size_t i = 0;
    Lock();
    while (str[i])
        putchar(str[i++], NULL);
    Unlock();
}

int printf(const char* fmt, ...)
{
    va_list args;
    int ret_val;

    va_start(args, fmt);
    ret_val = vprintf(fmt, args);
    va_end(args);
    // Swap out the buffer in draw
    graphics::swap();

    return ret_val;
}

} // !console
