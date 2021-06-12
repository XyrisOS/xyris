/**
 * @file graphics.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 */
#include <dev/vga/graphics.hpp>
#include <dev/vga/fb.hpp>
// Types
#include <stddef.h>
#include <stdint.h>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// System library functions
#include <sys/panic.hpp>
#include <lib/assert.hpp>
#include <lib/stdio.hpp>
// Bootloader
#include <boot/Handoff.hpp>
// Debug
#include <dev/serial/rs232.hpp>

namespace fb {

FramebufferInfo fbInfo;
// Cached values
static void* addr = NULL;
static uint32_t width = 0;
static uint32_t height = 0;
static uint16_t bpp = 0;
static uint32_t pitch = 0;
static bool initialized = false;

void init(FramebufferInfo info)
{
    fbInfo = info;
    // Ensure valid info is provided
    if (fbInfo.getAddress()) {
        // Cache commonly used values
        addr = fbInfo.getAddress();
        width = fbInfo.getWidth();
        height = fbInfo.getHeight();
        bpp = fbInfo.getBPP();
        pitch = fbInfo.getPitch();
        // Map in the framebuffer
        rs232_print("Mapping framebuffer...\n");
        for (uintptr_t page = (uintptr_t)addr & PAGE_ALIGN;
            page < ((uintptr_t)addr) + (pitch * height);
            page += PAGE_SIZE)
        {
            map_kernel_page(VADDR(page), page);
        }

        initialized = true;
    }
}

void pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized) { return; }
    // Reference: SkiftOS (Graphics.cpp)
    // Special thanks to the SkiftOS contributors.
    if ((x <= width) && (y <= height))
    {
        uint8_t *pixel = ((uint8_t*)addr + (y * pitch) + (x * (bpp / 8)));
        rs232_printf("pixel 0x%08X @ 0x%08X\n", color, pixel);

        pixel[0] = (color >> 0) & 0xff;
        pixel[1] = (color >> 8) & 0xff;
        pixel[2] = (color >> 16) & 0xff;
    }
}

void putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized) { return; }
    // Reference: SkiftOS (Graphics.cpp)
    // Special thanks to the SkiftOS contributors.
    if ((x <= width) && (y <= height))
    {
        for (uint32_t i = 0; i < w; i++)
        {
            if (x + i > width || x + i < width) { continue; }
            for (uint32_t j = 0; j < h; j++)
            {
                // Slow, but good for debugging.
                pixel(i, j, color);
            }
        }
    }
}

};