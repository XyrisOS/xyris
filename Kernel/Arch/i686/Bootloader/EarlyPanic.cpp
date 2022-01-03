/**
 * @file panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief Early CGA-mode panic (no framebuffer)
 * @version 0.3
 * @date 2020-10-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */

#include <Arch/Arch.hpp>
#include <Arch/i686/Bootloader/EarlyPanic.hpp>
#include <Support/sections.hpp>

#define TTY_WIDTH   80
#define TTY_HEIGHT  25
#define VGA_COLOR(bg, fg)   (uint16_t)(((bg)<<4)|((fg)&0xF))
#define VGA_CHAR(ch, co)    (uint16_t)((ch)|((co)<<8))

enum bios_color : uint16_t {
    BIOS_Black           = 0,
    BIOS_Blue            = 1,
    BIOS_Green           = 2,
    BIOS_Cyan            = 3,
    BIOS_Red             = 4,
    BIOS_Magenta         = 5,
    BIOS_Brown           = 6,
    BIOS_LightGrey       = 7,
    BIOS_DarkGrey        = 8,
    BIOS_LightBlue       = 9,
    BIOS_LightGreen      = 10,
    BIOS_LightCyan       = 11,
    BIOS_LightRed        = 12,
    BIOS_LightMagenta    = 13,
    BIOS_Yellow          = 14,
    BIOS_White           = 15
};

extern "C"
__attribute__((section(".early_text")))
void EarlyPanic(const char *str)
{
    volatile uint16_t* where;
    uint16_t* biosVGABuffer = (uint16_t*)0x000B8000;
    int x = 0;
	int y = 0;
    // Clear the screen
    for (int i = 0; i < TTY_WIDTH; i++) {
        for (int j = 0; j < TTY_HEIGHT; j++) {
            where = biosVGABuffer + (j * TTY_WIDTH + i);
            *where = VGA_CHAR(' ', VGA_COLOR(BIOS_Black, BIOS_White));
        }
    }
    // For each character in the string
    for (int i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            // Newline
            case '\n':
                x = 0;
                y++;
                break;
            // Anything else
            default:
                where = biosVGABuffer + (y * TTY_WIDTH + x);
                *where = VGA_CHAR(str[i], VGA_COLOR(BIOS_Red, BIOS_White));
                x++;
                break;
        }
    }

    Arch::haltAndCatchFire();
}
