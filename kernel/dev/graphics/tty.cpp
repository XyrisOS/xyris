/**
 * @file tty.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief TTY is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * kprintf().
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#include <dev/graphics/framebuffer.hpp>
#include <dev/graphics/graphics.hpp>
#include <dev/graphics/tty.hpp>
#include <lib/stdio.hpp>
#include <stddef.h>

#define INDICATOR_WIDTH 8
#define INDICATOR_HEIGHT 16

// Coorinate trackers
uint8_t tty_coords_x = 0;
uint8_t tty_coords_y = 0;
// VGA colors (defaults are white on black)
tty_vga_color color_back = VGA_Black;
tty_vga_color color_fore = VGA_White;
// Default colors set by tty_clear()
tty_vga_color reset_back = VGA_Black;
tty_vga_color reset_fore = VGA_White;

namespace console {

void Write(char* str, uint32_t fore, uint32_t back)
{
    (void)str;
    (void)fore;
    (void)back;
}

}
