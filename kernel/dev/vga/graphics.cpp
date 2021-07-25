/**
 * @file graphics.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief
 * @version 0.2
 * @date 2021-07-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *     https://wiki.osdev.org/Double_Buffering
 *     https://github.com/skiftOS/skift/blob/main/kernel/system/graphics/Graphics.cpp
 * 
 */
#include <dev/vga/framebuffer.hpp>
#include <dev/vga/graphics.hpp>
// Types
#include <stddef.h>
#include <stdint.h>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// System library functions
#include <lib/assert.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <boot/Handoff.hpp>
#include <dev/serial/rs232.hpp>

namespace graphics {

FramebufferInfo fbInfo;
static void* backbuffer = NULL;
static bool initialized = false;

bool isInitialized() { return initialized; }

static void testPattern() {
    const uint32_t BAR_COLOR[8] =
    {
        0xFFFFFF,  // 100% White
        0xFFFF00,  // Yellow
        0x00FFFF,  // Cyan
        0x00FF00,  // Green
        0xFF00FF,  // Magenta
        0xFF0000,  // Red
        0x0000FF,  // Blue
        0x000000,  // Black
    };

    resetDoubleBuffer();
    // Generate complete frame
    unsigned columnWidth = fbInfo.getWidth() / 8;
    for (unsigned y = 0; y < fbInfo.getHeight(); y++)
    {
        for (unsigned x = 0; x < fbInfo.getWidth(); x++)
        {
            unsigned col_idx = x / columnWidth;
            pixel(x, y, BAR_COLOR[col_idx]);
        }
    }
    // Show modification in the Screen
    swap();
}

void init(FramebufferInfo info)
{
    fbInfo = info;
    // Ensure valid info is provided
    if (!fbInfo.getAddress())
        return;
    // Map in the framebuffer
    RS232::printf("Mapping framebuffer...\n");
    for (uintptr_t page = (uintptr_t)fbInfo.getAddress() & PAGE_ALIGN;
         page < (uintptr_t)fbInfo.getAddress() + (fbInfo.getPitch() * fbInfo.getHeight());
         page += PAGE_SIZE) {
        map_kernel_page(VADDR(page), page);
    }
    // Alloc the backbuffer
    backbuffer = malloc(fbInfo.getPitch() * fbInfo.getHeight());
    memcpy(backbuffer, fbInfo.getAddress(), fbInfo.getPitch() * fbInfo.getHeight());

    initialized = true;
    testPattern();
}

void pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized)
        return;
    if ((x <= fbInfo.getWidth()) && (y <= fbInfo.getHeight())) {
        // Special thanks to the SkiftOS contributors.
        uint8_t* pixel = (uint8_t*)backbuffer + (y * fbInfo.getPitch()) + (x * fbInfo.getPixelWidth());
        // Pixel information
        pixel[0] = (color >> fbInfo.getBlueMaskShift()) & 0xff;  // B
        pixel[1] = (color >> fbInfo.getGreenMaskShift()) & 0xff; // G
        pixel[2] = (color >> fbInfo.getRedMaskShift()) & 0xff;   // R
        // Additional pixel information
        if (fbInfo.getPixelWidth() == 4)
            pixel[3] = 0x00;
    }
}

void putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized)
        return;
    for (uint32_t curr_x = x; curr_x <= x + w; curr_x++) {
        for (uint32_t curr_y = y; curr_y <= y + h; curr_y++) {
            // Extremely slow but good for debugging
            pixel(curr_x, curr_y, color);
        }
    }
}

void resetDoubleBuffer()
{
    memset(backbuffer, 0, (fbInfo.getPitch() * fbInfo.getHeight()));
}

void swap()
{
    memcpy(fbInfo.getAddress(), backbuffer, (fbInfo.getPitch() * fbInfo.getHeight()));
}

} // !namespace graphics
