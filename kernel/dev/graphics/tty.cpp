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

// Coorinate trackers
uint8_t tty_coords_x = 0;
uint8_t tty_coords_y = 0;
// VGA colors (defaults are white on black)
tty_vga_color color_back = VGA_Black;
tty_vga_color color_fore = VGA_White;
// Default colors set by tty_clear()
tty_vga_color reset_back = VGA_Black;
tty_vga_color reset_fore = VGA_White;

void tty_shift_up()
{
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

void tty_clear(tty_vga_color fore, tty_vga_color back)
{
    tty_coords_x = 0;
    tty_coords_y = 0;
    color_fore = reset_fore = fore;
    color_back = reset_back = back;
    graphics::Framebuffer* fb = graphics::getFramebuffer();
    graphics::putrect(0, 0, fb->getWidth(), fb->getHeight(), reset_back);
}

void set_indicator(tty_vga_color color)
{
    graphics::putrect(0, 0, 16, 32, color);
}
