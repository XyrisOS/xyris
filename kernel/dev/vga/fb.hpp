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
#pragma once
#include <stdint.h>

namespace FB {

enum FramebufferMemoryModel {
    Undefined_FBMM = 0,
    RGB_FBMM = 1,
};

class FramebufferInfo {
public:
    // Constructors
    FramebufferInfo();
    FramebufferInfo(uint32_t width, uint32_t height, uint16_t depth, uint32_t pitch, void* addr);
    FramebufferInfo(uint32_t width, uint32_t height,
                    uint16_t depth, uint32_t pitch,
                    void* addr, FramebufferMemoryModel model,
                    uint8_t redMaskSize, uint8_t redMaskShift,
                    uint8_t greenMaskSize, uint8_t greenMaskShift,
                    uint8_t blueMaskSize, uint8_t blueMaskShift);
    // Getters
    void* getAddress()                          { return _addr; }
    uint32_t getWidth()                         { return _width; }
    uint32_t getHeight()                        { return _height; }
    uint16_t getDepth()                         { return _depth; }
    uint32_t getPitch()                         { return _pitch; }
    uint8_t getRedMaskSize()                    { return _redMaskSize; }
    uint8_t getRedMaskShift()                   { return _redMaskShift; }
    uint8_t getGreenMaskSize()                  { return _greenMaskSize; }
    uint8_t getGreenMaskShift()                 { return _greenMaskShift; }
    uint8_t getBlueMaskSize()                   { return _blueMaskSize; }
    uint8_t getBlueMaskShift()                  { return _blueMaskShift; }
    FramebufferMemoryModel getModel()           { return _memoryModel; }
    // Setters
    void setWidth(uint16_t val)                 { _width = val; }
    void setHeight(uint16_t val)                { _height = val; }
    void setDepth(uint16_t val)                 { _depth = val; }
    void setPitch(uint16_t val)                 { _pitch = val; }
    void setRedMaskSize(uint8_t val)            { _redMaskSize = val; }
    void setRedMaskShift(uint8_t val)           { _redMaskShift = val; }
    void setGreenMaskSize(uint8_t val)          { _greenMaskSize = val; }
    void setGreenMaskShift(uint8_t val)         { _greenMaskShift = val; }
    void setBlueMaskSize(uint8_t val)           { _blueMaskSize = val; }
    void setBlueMaskShift(uint8_t val)          { _blueMaskShift = val; }
    void setModel(FramebufferMemoryModel val)   { _memoryModel = val; }

private:
    void* _addr;                                // physical framebuffer address
    uint32_t _width;                            // number pixels on a horizontal line
    uint32_t _height;                           // number horizontal lines present
    uint16_t _depth;                            // color depth (same as bits per pixel)
    uint32_t _pitch;                            // number bytes of VRAM to skip to go one pixel down
    uint8_t _redMaskSize;
    uint8_t _redMaskShift;
    uint8_t _greenMaskSize;
    uint8_t _greenMaskShift;
    uint8_t _blueMaskSize;
    uint8_t _blueMaskShift;
    FramebufferMemoryModel _memoryModel;
};

} // !namespace FB
