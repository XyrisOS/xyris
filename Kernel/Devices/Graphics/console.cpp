/**
 * @file console.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Framebuffer console
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#include <Devices/Graphics/console.hpp>
#include <Devices/Graphics/font.hpp>
#include <Devices/Graphics/framebuffer.hpp>
#include <Devices/Graphics/graphics.hpp>
#include <Library/stdio.hpp>
#include <Locking/Mutex.hpp>
#include <stddef.h>

namespace Console {

#define PUSH_VAL(VAL) ansiValues[ansiValuesIdx++] = (VAL)
#define POP_VAL() ansiValues[--ansiValuesIdx]
#define CLEAR_VALS() ansiValuesIdx = 0
#define ESC ('\033')
#define TAB_WIDTH 4u

enum vgaColor : uint32_t {
    VGA_Black = 0x000000,
    VGA_Blue = 0x0000AA,
    VGA_Green = 0x00AA00,
    VGA_Cyan = 0x00AAAA,
    VGA_Red = 0xAA0000,
    VGA_Magenta = 0xAA00AA,
    VGA_Brown = 0xAA5500,
    VGA_LightGrey = 0xAAAAAA,
    VGA_DarkGrey = 0x555555,
    VGA_LightBlue = 0x5555FF,
    VGA_LightGreen = 0x55FF55,
    VGA_LightCyan = 0x55FFFF,
    VGA_LightRed = 0xFF5555,
    VGA_LightMagenta = 0xFF55FF,
    VGA_Yellow = 0xFFFF55,
    VGA_White = 0xFFFFFF,
};

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

// Coorinate trackers
static uint8_t cursorX = 0;
static uint8_t cursorY = 0;
static uint32_t colorBack = VGA_Black;
static uint32_t colorFore = VGA_White;
static uint32_t resetBack = VGA_Black;
static uint32_t resetFore = VGA_White;

// ANSI State and value storage
enum ansiStateType {
    Normal,
    Esc,
    Bracket,
    Value
};

static uint16_t ansiVal = 0;
static size_t ansiValuesIdx = 0;
static uint16_t ansiValues[8] = { 0 };
static ansiStateType ansiState = Normal;
static uint32_t ansiCursorX = 0;
static uint32_t ansiCursorY = 0;
static uint32_t ansiVGATable[16] = {
    VGA_Black, VGA_Red, VGA_Green, VGA_Brown, VGA_Blue,
    VGA_Magenta, VGA_Cyan, VGA_LightGrey, VGA_DarkGrey, VGA_LightRed,
    VGA_LightGreen, VGA_Yellow, VGA_LightBlue, VGA_LightMagenta,
    VGA_LightCyan, VGA_White
};

static Mutex ttyLock;

static void Lock()
{
    ttyLock.Lock();
}

static void Unlock()
{
    ttyLock.Unlock();
}

static int putchar(unsigned c, void** ptr)
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
            ansiCursorX = cursorX;
            ansiCursorY = cursorY;
            goto normal;
        } else if (c == 'u') { // Restore cursor position attribute
            cursorX = ansiCursorX;
            cursorY = ansiCursorY;
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
            cursorX = (uint8_t)POP_VAL();
            cursorY = (uint8_t)POP_VAL();
            goto normal;
        } else if (c == 'J') { // Clear screen attribute
            // The proper code is ESC[2J
            if (ansiVal != 2) {
                goto error;
            }
            // Clear by resetting the double buffer and swapping.
            // TODO: Find a better way to do this?
            debugf("Clearing screen\n");
            Graphics::resetDoubleBuffer();
            Graphics::swap();
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
        if (cursorX > 0) {
            cursorX--;
        }
        break;
    // Newline
    case '\n':
        cursorX = 0;
        cursorY++;
        break;
    case '\t':
        while (++cursorX % TAB_WIDTH)
            ;
        break;
    // Carriage return
    case '\r':
        cursorX = 0;
        break;
    // Anything else (visible characters)
    default:
        Graphics::Font::Draw(c, cursorX++, cursorY, colorFore, colorBack);
    }

    // Move to the next line
    // TODO: Get width of "screen" (replace 80 with width) (#275)
    if (cursorX >= 80) {
        debugf("Move to the next line\n");
        cursorX = 0;
        cursorY++;
    }
    // Clear the screen
    // TODO: Get height of "screen" (replace 25 with height) (#275)
    if (cursorY >= 25) {
        debugf("Shift up the screen\n");
        //TODO: Shift text up and reset the bottom line
        cursorX = 0;
        cursorY = 25 - 1;
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

int vprintf(const char* fmt, va_list args)
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
    Graphics::swap();

    return ret_val;
}

void reset(uint32_t fore, uint32_t back)
{
    cursorX = cursorY = 0;
    colorBack = resetBack = back;
    colorFore = resetFore = fore;
}

void reset()
{
    cursorX = cursorY = 0;
    colorBack = resetBack;
    colorFore = resetFore;
}

} // !console
