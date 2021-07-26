/**
 * @file early_panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @version 0.3
 * @date 2020-10-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */

#include <arch/arch.hpp>
#include <meta/sections.hpp>
#include <meta/compiler.hpp>
#include <dev/graphics/tty.hpp>

#define VGA_COLOR(bg, fg) (uint16_t)(((bg)<<4)|((fg)&0xF))
#define VGA_CHAR(ch, co) (uint16_t)((ch)|((co)<<8))

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

// Provide a function prototype to make the compiler warnings happy. Don't
// want to make it public though, so we won't put it in the panic header.
extern "C" void early_panic(const char *str);

extern "C" void
SECTION(".early_text")
OPTIMIZE(0)
early_panic(const char *str) {
    volatile uint16_t* where;
    int x = 0;
	int y = 0;
    // Clear the screen
    for (int i = 0; i < X86_TTY_WIDTH; i++) {
        for (int j = 0; j < X86_TTY_HEIGHT; j ++) {
            where = x86_bios_vga_mem + (j * X86_TTY_WIDTH + i);
            *where = VGA_CHAR(' ', VGA_COLOR(BIOS_Black, BIOS_White));
        }
    }
    // For each character in the string
    for (int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            // Newline
            case '\n':
                x = 0;
                y++;
                break;
            // Anything else
            default:
                where = x86_bios_vga_mem + (y * X86_TTY_WIDTH + x);
                *where = VGA_CHAR(str[i], VGA_COLOR(BIOS_Red, BIOS_White));
                x++;
                break;
        }
    }
}

