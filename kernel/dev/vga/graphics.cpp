/**
 * @file graphics.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Graphics management and control
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
#include <boot/Handoff.hpp>
#include <dev/serial/rs232.hpp>
#include <lib/assert.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>

namespace graphics {

Framebuffer fb;
static void* backbuffer = NULL;
static bool initialized = false;

bool isInitialized() { return initialized; }

Framebuffer* getFramebuffer()
{
    return &fb;
}

void init(Framebuffer info)
{
    info = info;
    // Ensure valid info is provided
    if (!info.getAddress())
        return;
    // Map in the framebuffer
    RS232::printf("Mapping framebuffer...\n");
    for (uintptr_t page = (uintptr_t)info.getAddress() & PAGE_ALIGN;
         page < (uintptr_t)info.getAddress() + (info.getPitch() * info.getHeight());
         page += PAGE_SIZE) {
        map_kernel_page(VADDR(page), page);
    }
    // Alloc the backbuffer
    backbuffer = malloc(info.getPitch() * info.getHeight());
    memcpy(backbuffer, info.getAddress(), info.getPitch() * info.getHeight());

    initialized = true;
}

void pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized)
        return;
    if ((x <= fb.getWidth()) && (y <= fb.getHeight())) {
        // Special thanks to the SkiftOS contributors.
        uint8_t* pixel = (uint8_t*)backbuffer + (y * fb.getPitch()) + (x * fb.getPixelWidth());
        // Pixel information
        pixel[0] = (color >> fb.getBlueMaskShift()) & 0xff;  // B
        pixel[1] = (color >> fb.getGreenMaskShift()) & 0xff; // G
        pixel[2] = (color >> fb.getRedMaskShift()) & 0xff;   // R
        // Additional pixel information
        if (fb.getPixelWidth() == 4)
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
    // Swap after doing a large operation
    swap();
}

void resetDoubleBuffer()
{
    memset(backbuffer, 0, (fb.getPitch() * fb.getHeight()));
}

void swap()
{
    memcpy(fb.getAddress(), backbuffer, (fb.getPitch() * fb.getHeight()));
}

} // !namespace graphics
