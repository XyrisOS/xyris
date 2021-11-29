/**
 * @file framebuffer.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#include <Support/compiler.hpp>

namespace Graphics {

enum FramebufferMemoryModel {
    Undefined_FBMM = 0,
    RGB_FBMM = 1,
};

class Framebuffer {
public:
    // Constructors
    Framebuffer();
    Framebuffer(uint32_t width, uint32_t height, uint16_t depth, uint32_t pitch, void* addr);
    Framebuffer(uint32_t width, uint32_t height,
                    uint16_t depth, uint32_t pitch,
                    void* addr, FramebufferMemoryModel model,
                    uint8_t redMaskSize, uint8_t redMaskShift,
                    uint8_t greenMaskSize, uint8_t greenMaskShift,
                    uint8_t blueMaskSize, uint8_t blueMaskShift);
    // Getters
    ALWAYS_INLINE void* getAddress()                          { return _addr; }
    ALWAYS_INLINE uint32_t getWidth()                         { return _width; }
    ALWAYS_INLINE uint32_t getHeight()                        { return _height; }
    ALWAYS_INLINE uint16_t getDepth()                         { return _depth; }
    ALWAYS_INLINE uint32_t getPitch()                         { return _pitch; }
    ALWAYS_INLINE uint8_t getRedMaskSize()                    { return _redMaskSize; }
    ALWAYS_INLINE uint8_t getRedMaskShift()                   { return _redMaskShift; }
    ALWAYS_INLINE uint8_t getGreenMaskSize()                  { return _greenMaskSize; }
    ALWAYS_INLINE uint8_t getGreenMaskShift()                 { return _greenMaskShift; }
    ALWAYS_INLINE uint8_t getBlueMaskSize()                   { return _blueMaskSize; }
    ALWAYS_INLINE uint8_t getBlueMaskShift()                  { return _blueMaskShift; }
    ALWAYS_INLINE uint8_t getPixelWidth()                     { return _depth / 8; }
    ALWAYS_INLINE FramebufferMemoryModel getModel()           { return _memoryModel; }
    // Setters
    ALWAYS_INLINE void setWidth(uint16_t val)                 { _width = val; }
    ALWAYS_INLINE void setHeight(uint16_t val)                { _height = val; }
    ALWAYS_INLINE void setDepth(uint16_t val)                 { _depth = val; }
    ALWAYS_INLINE void setPitch(uint16_t val)                 { _pitch = val; }
    ALWAYS_INLINE void setRedMaskSize(uint8_t val)            { _redMaskSize = val; }
    ALWAYS_INLINE void setRedMaskShift(uint8_t val)           { _redMaskShift = val; }
    ALWAYS_INLINE void setGreenMaskSize(uint8_t val)          { _greenMaskSize = val; }
    ALWAYS_INLINE void setGreenMaskShift(uint8_t val)         { _greenMaskShift = val; }
    ALWAYS_INLINE void setBlueMaskSize(uint8_t val)           { _blueMaskSize = val; }
    ALWAYS_INLINE void setBlueMaskShift(uint8_t val)          { _blueMaskShift = val; }
    ALWAYS_INLINE void setModel(FramebufferMemoryModel val)   { _memoryModel = val; }

private:
    void* _addr;                                // physical framebuffer address
    uint32_t _width;                            // number pixels on a horizontal line
    uint32_t _height;                           // number horizontal lines present
    uint16_t _depth;                            // color depth (same as bits per pixel)
    uint32_t _pitch;                            // number bytes of VRAM in each line
    uint8_t _redMaskSize;
    uint8_t _redMaskShift;
    uint8_t _greenMaskSize;
    uint8_t _greenMaskShift;
    uint8_t _blueMaskSize;
    uint8_t _blueMaskShift;
    FramebufferMemoryModel _memoryModel;
};

/**
 * @brief Get a pointer to the active framebuffer
 *
 * @return Framebuffer* Pointer to active framebuffer
 */
Framebuffer* getFramebuffer();

}; // !namespace graphics
