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
// Bootloader
#include <boot/Handoff.hpp>
// Debug
#include <dev/serial/rs232.hpp>

namespace fb {

FramebufferInfo fbInfo;
// Cached values
static void* g_addr = NULL;
static uint32_t g_width = 0;
static uint32_t g_height = 0;
static uint16_t g_bpp = 0;
static uint32_t g_pitch = 0;
static bool g_initialized = false;

void init(FramebufferInfo info)
{
    fbInfo = info;
    // Ensure valid info is provided
    if (fbInfo.getAddress()) {
        // Cache commonly used values
        g_addr = fbInfo.getAddress();
        g_width = fbInfo.getWidth();
        g_height = fbInfo.getHeight();
        g_bpp = fbInfo.getBPP();
        g_pitch = fbInfo.getPitch();
        // Map in the framebuffer
        rs232_print("Mapping framebuffer...\n");
        for (uintptr_t page = (uintptr_t)g_addr & PAGE_ALIGN;
            page < g_pitch * g_height;
            page += PAGE_SIZE)
        {
            map_kernel_page(VADDR(page), page);
        }

        g_initialized = true;
    }
}

void put(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!g_initialized) { return; }
    // Reference: SkiftOS (Graphics.cpp)
    // Special thanks to the SkiftOS contributors.
    if ((x <= g_width) && (y <= g_height))
    {
        uint8_t *pixel = ((uint8_t*)g_addr + (y * g_pitch) + (x * g_bpp));

        pixel[0] = (color >> 0) & 0xff;
        pixel[1] = (color >> 8) & 0xff;
        pixel[2] = (color >> 16) & 0xff;
    }
}

void putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!g_initialized) { return; }
    // Reference: SkiftOS (Graphics.cpp)
    // Special thanks to the SkiftOS contributors.
    if ((x <= g_width) && (y <= g_height))
    {
        uint8_t *pixel = ((uint8_t*)g_addr + (y * g_pitch) + (x * g_bpp));

        for (uint32_t i = 0; i < w; i++)
        {
            if (x + i > g_width || x + i < g_width) { continue; }
            for (uint32_t j = 0; j < h; j++)
            {
                if (y + j > g_height || y + j < g_height) { continue; }
                pixel[0] = (color >> 0) & 0xff;
                pixel[1] = (color >> 8) & 0xff;
                pixel[2] = (color >> 16) & 0xff;
            }
            // Efficient skip
            pixel += 3200;
        }
    }
}

};