/**
 * @file fb.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <dev/vga/fb.hpp>
// Types
#include <stddef.h>
#include <stdint.h>

namespace FB {

FramebufferInfo::FramebufferInfo()
    : _addr(NULL)
    , _width(0)
    , _height(0)
    , _depth(0)
    , _pitch(0)
    , _redMaskSize(0)
    , _redMaskShift(0)
    , _greenMaskSize(0)
    , _greenMaskShift(0)
    , _blueMaskSize(0)
    , _blueMaskShift(0)
    , _memoryModel(Undefined_FBMM)
{
    // Default constructor.
}

FramebufferInfo::FramebufferInfo(uint32_t width, uint32_t height, uint16_t depth, uint32_t pitch, void* addr)
    : _addr(addr)
    , _width(width)
    , _height(height)
    , _depth(depth)
    , _pitch(pitch)
    , _redMaskSize(0)
    , _redMaskShift(0)
    , _greenMaskSize(0)
    , _greenMaskShift(0)
    , _blueMaskSize(0)
    , _blueMaskShift(0)
    , _memoryModel(Undefined_FBMM)
{
    // Common parameters constructor
}

FramebufferInfo::FramebufferInfo(uint32_t width, uint32_t height,
                                 uint16_t depth, uint32_t pitch,
                                 void* addr, FramebufferMemoryModel model,
                                 uint8_t redMaskSize, uint8_t redMaskShift,
                                 uint8_t greenMaskSize, uint8_t greenMaskShift,
                                 uint8_t blueMaskSize, uint8_t blueMaskShift)
    : _addr(addr)
    , _width(width)
    , _height(height)
    , _depth(depth)
    , _pitch(pitch)
    , _redMaskSize(redMaskSize)
    , _redMaskShift(redMaskShift)
    , _greenMaskSize(greenMaskSize)
    , _greenMaskShift(greenMaskShift)
    , _blueMaskSize(blueMaskSize)
    , _blueMaskShift(blueMaskShift)
    , _memoryModel(model)
{
    // All parameters constructor
}

} // !namespace FB
