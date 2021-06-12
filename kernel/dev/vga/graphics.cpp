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
static uint16_t depth = 0;
static uint32_t pitch = 0;
static uint32_t pixelwidth = 0;
static uint8_t r_size = 0;
static uint8_t r_shift = 0;
static uint8_t g_size = 0;
static uint8_t g_shift = 0;
static uint8_t b_size = 0;
static uint8_t b_shift = 0;
static bool initialized = false;

bool isInitialized() { return initialized; }

void init(FramebufferInfo info)
{
    fbInfo = info;
    // Ensure valid info is provided
    if (fbInfo.getAddress()) {
        // Cache framebuffer values in order to avoid
        // function calls when plotting pixels.
        addr = fbInfo.getAddress();
        width = fbInfo.getWidth();
        height = fbInfo.getHeight();
        depth = fbInfo.getDepth();
        pitch = fbInfo.getPitch();
        r_size = fbInfo.getRedMaskSize();
        r_shift = fbInfo.getRedMaskShift();
        g_size = fbInfo.getGreenMaskSize();
        g_shift = fbInfo.getGreenMaskShift();
        b_size = fbInfo.getBlueMaskSize();
        b_shift = fbInfo.getBlueMaskShift();
        pixelwidth = (depth / 8);
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
        uint8_t *pixel = ((uint8_t*)addr + (y * pitch) + (x * pixelwidth));
        // Pixel information
        pixel[0] = (color >> b_shift) & 0xff;   // B
        pixel[1] = (color >> g_shift) & 0xff;   // G
        pixel[2] = (color >> r_shift) & 0xff;   // R
        // Additional pixel information
        if (pixelwidth == 4) { pixel[3] = 0x00; }
    }
}

void putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized) { return; }
    for (uint32_t curr_x = x; curr_x <= x + w; curr_x++) {
        for (uint32_t curr_y = y; curr_y <= y + h; curr_y++) {
            // Extremely slow but good for debugging
            pixel(curr_x, curr_y, color);
        }
    }
}

};