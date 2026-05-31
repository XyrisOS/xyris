/**
 * @file graphics.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @author Amilie Baker (amiliefn@gmail.com)
 * @brief Graphics management and control
 * @version 0.2
 * @date 2021-07-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *     https://wiki.osdev.org/Double_Buffering
 *     https://github.com/skiftOS/skift/blob/main/kernel/system/Graphics/Graphics.cpp
 *     https://en.cppreference.com/cpp/thread/lock_guard
 *
 */
#include <Devices/Graphics/graphics.hpp>
#include <stddef.h>
#include <stdint.h>
#include <Memory/heap.hpp>
#include <Memory/paging.hpp>
#include <Bootloader/Handoff.hpp>
#include <Library/stdio.hpp>
#include <Library/string.hpp>
#include <Logger.hpp>
#include <Locking/RAII.hpp>
#include <Arch/i686/timer.hpp>

namespace Graphics {
static Framebuffer* info = NULL;
static void* backbuffer = NULL;
static bool initialized = false;

void init(Framebuffer* fb)
{
    // Get the framebuffer info
    if (!(info = fb))
        return;
    // Ensure valid info is provided
    if (!info->getAddress())
        return;
    // Map in the framebuffer
    Logger::Debug(__func__, "==== MAP FRAMEBUFFER ====");
    Memory::mapKernelRangeVirtual(Memory::Section(
        (uintptr_t)info->getAddress(),
        (info->getPitch() * info->getHeight())
    ));
    // Alloc the backbuffer
    Mutex mutex_backbufferAccess("backbuffer"); // <- And the mutex
    backbuffer = malloc(info->getPitch() * info->getHeight());
    memcpy(backbuffer, info->getAddress(), info->getPitch() * info->getHeight());
    initialized = true;
}

static void _pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure framebuffer information exists
    if (!initialized)
        return;
    if ((x <= info->getWidth()) && (y <= info->getHeight())) {
        // Special thanks to the SkiftOS contributors.
        uint8_t* pixel = (uint8_t*)backbuffer + (y * info->getPitch()) + (x * info->getPixelWidth());
        // Pixel information
        pixel[0] = (color >> info->getBlueMaskShift()) & 0xff;  // B
        pixel[1] = (color >> info->getGreenMaskShift()) & 0xff; // G
        pixel[2] = (color >> info->getRedMaskShift()) & 0xff;   // R
        // Additional pixel information
        if (info->getPixelWidth() == 4)
            pixel[3] = 0x00;
    }
}

void pixel(uint32_t x, uint32_t y, uint32_t color)
{
    RAIIMutex lock(mutex_backbufferAccess);
    _pixel(x, y, color);
}

static void _resetDoubleBuffer()
{
    if (!initialized){
        Logger::Debug(__func__, "releasing");
        return;
    }
    memset(backbuffer, 0, (info->getPitch() * info->getHeight()));
}

void resetDoubleBuffer(){
    Logger::Debug(__func__, "aquiring lock");
    RAIIMutex lock(mutex_backbufferAccess);
    Logger::Debug(__func__, "locked");

    _resetDoubleBuffer();
}

static void _swap()
{
    if (!initialized){
        Logger::Debug(__func__, "releasing");
        return;
    }
    memcpy(info->getAddress(), backbuffer, (info->getPitch() * info->getHeight()));
}

void swap()
{
    Logger::Debug(__func__, "aquiring lock");
    RAIIMutex lock(mutex_backbufferAccess);
    Logger::Debug(__func__, "locked");

    _swap();
}

void putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    RAIIMutex lock(mutex_backbufferAccess);

    // Ensure framebuffer information exists
    if (!initialized)
        return;
    for (uint32_t curr_x = x; curr_x <= x + w; curr_x++) {
        for (uint32_t curr_y = y; curr_y <= y + h; curr_y++) {
            // Extremely slow but good for debugging
            _pixel(curr_x, curr_y, color);
        }
    }
    // Swap after doing a large operation
    _swap();
}
}

// !namespace graphics