/**
 * @file early_panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @version 0.3
 * @date 2020-10-11
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

#include <arch/arch.hpp>
#include <dev/tty/tty.hpp>

// Provide a function prototype to make the compiler warnings happy. Don't
// want to make it public though, so we won't put it in the panic header.
extern "C" void early_panic(const char *str);

extern "C" void
__attribute__ ((section(".early_text")))
__attribute__((optimize("O0")))
early_panic(const char *str) {
    volatile uint16_t* where;
    int x = 0;
	int y = 0;
    // Clear the screen
    for (int i = 0; i < X86_TTY_WIDTH; i++) {
        for (int j = 0; j < X86_TTY_HEIGHT; j ++) {
            where = x86_bios_vga_mem + (j * X86_TTY_WIDTH + i);
            *where = (uint16_t)VGA_CHAR(' ', VGA_COLOR(VGA_Black, VGA_White));
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
                *where = (uint16_t)VGA_CHAR(str[i], VGA_COLOR(VGA_Red, VGA_White));
                x++;
                break;
        }
    }
}

