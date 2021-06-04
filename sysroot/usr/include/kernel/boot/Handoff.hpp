/**
 * @file Handoff.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2021-06-02
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#include <stdint.h>
#include <stdbool.h>

namespace Boot {

enum HandoffFirmwareType {
    BIOS = 0,
    UEFI = 1,
};

enum HandoffBootloaderType {
    Multiboot2 = 0,
    Stivale2 = 1,
};

// Unused for now.
class HandoffRSDPDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffRSDPDescriptor();
    ~HandoffRSDPDescriptor();
    // Getters
    const char* getSignature()          { return _signature; }
    uint8_t getChecksum()               { return _checksum; }
    const char* getOEMID()              { return _OEMID; }
    uint8_t getRevision()               { return _revision; }
    uint32_t getRSDTAddress()           { return _rsdtAddress; }

private:
    char _signature[8];
    uint8_t _checksum;
    char _OEMID[6];
    uint8_t _revision;
    uint32_t _rsdtAddress;
};

// Unused for now.
class HandoffCPUDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffCPUDescriptor();
    ~HandoffCPUDescriptor();
    // Getters
    uint32_t getCPUID()                 { return _cpuID; }
    uint32_t getLAPICID()               { return _lapicID; }
    void* getStackAddr()                { return _stackAddr; }
    void* getGotoAddr()                 { return _gotoAddr; }
    void* getArguments()                { return _arguments; }

private:
    uint32_t _cpuID;
    uint32_t _lapicID;
    void* _stackAddr;
    void* _gotoAddr;
    void* _arguments;
};

// Unused for now.
class HandoffSMPDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffSMPDescriptor();
    ~HandoffSMPDescriptor();
    // Getters
    bool getX2Available()               { return _x2Available; }
    uint32_t getLAPICID()               { return _lapicID; }
    uint64_t getCPUCount()              { return _cpuCount; }

private:
    bool _x2Available;
    uint32_t _lapicID;
    uint64_t _cpuCount;
};

enum FramebufferMemoryModel {
    Undefined_FBMM = 0,
    RGB_FBMM = 1,
};

// TODO: Move this to the appropriate place when possible.
class FramebufferInfo {
public:
    // Constructors
    FramebufferInfo();
    FramebufferInfo(uint16_t width, uint16_t height, uint16_t depth, uint16_t pitch, void* addr);
    FramebufferInfo(uint16_t width, uint16_t height,
                    uint16_t depth, uint16_t pitch,
                    void* addr, FramebufferMemoryModel model,
                    uint8_t redMaskSize, uint8_t redMaskShift,
                    uint8_t greenMaskSize, uint8_t greenMaskShift,
                    uint8_t blueMaskSize, uint8_t blueMaskShift);
    // Getters
    uint16_t getWidth()                         { return _width; }
    uint16_t getHeight()                        { return _height; }
    uint16_t getDepth()                         { return _depth; }
    uint16_t getPitch()                         { return _pitch; }
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
    void* _addr;
    uint16_t _width;
    uint16_t _height;
    uint16_t _depth;
    uint16_t _pitch;
    uint8_t _redMaskSize;
    uint8_t _redMaskShift;
    uint8_t _greenMaskSize;
    uint8_t _greenMaskShift;
    uint8_t _blueMaskSize;
    uint8_t _blueMaskShift;
    FramebufferMemoryModel _memoryModel;
};

// TODO: Remaining information to be made obtainable
//  * PXE IP address (once we have a nice IP struct)
//  * Memory map layout (once we have new memory manager)
//  * Update Stivale2 to latest version & add missing
//  * Kernel modules (linked list of some sort?)
class Handoff {
public:
    // Constructors
    Handoff(void* handoff, uint32_t magic);
    ~Handoff();
    // Getters
    const char* getCmdLine()                { return _cmdline; }
    const void* getHandle()                 { return _handle; }
    uint64_t getEpoch()                     { return _epoch; }
    FramebufferInfo getFramebufferInfo()    { return _fbInfo; }
    HandoffBootloaderType getBootType()     { return _bootType; }
    HandoffFirmwareType getFirmwareType()   { return _fwType; }

private:
    void parseStivale2(void* handoff);
    void parseMultiboot2(void* handoff);

    const void* _handle;
    const char* _cmdline;
    const uint32_t _magic;
    uint64_t _epoch;
    FramebufferInfo _fbInfo;
    HandoffBootloaderType _bootType;
    HandoffFirmwareType _fwType;
};

}; // !namespace Boot
