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

extern "C" void
__attribute__ ((section(".early_text")))
__attribute__ ((optnone))
early_panic(const char *str) {
    volatile uint16_t* where;
    int x = 0;
	int y = 0;
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
                *where = VGA_CHAR(str[i], VGA_COLOR(VGA_Red, VGA_White));
                x++;
                break;
        }
    }
}

