/**
 * @file Handoff.hpp
 * @author Keeton Feave (keetonfeavel@cedarville.edu)
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
    HandoffRSDPDescriptor();
    ~HandoffRSDPDescriptor();
    const char* getSignature();
    uint8_t getChecksum();
    const char* getOEMID();
    uint8_t getRevision();
    uint32_t getRSDTAddress();

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
    HandoffCPUDescriptor();
    ~HandoffCPUDescriptor();

private:
    uint32_t cpuID;
    uint32_t lapicID;
    void* stackAddr;
    void* gotoAddr;
    void* arguments;
};

// Unused for now.
class HandoffSMPDescriptor {
public:
    HandoffSMPDescriptor();
    ~HandoffSMPDescriptor();

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
    FramebufferInfo();
    FramebufferInfo(uint16_t width, uint16_t height, uint16_t depth, uint16_t pitch, void* addr);
    ~FramebufferInfo();
    uint16_t getWidth();
    uint16_t getHeight();
    uint16_t getDepth();

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
    Handoff(void* handoff, uint32_t magic);
    ~Handoff();
    const char* getCmdLine();
    void* getHandle();
    uint64_t getEpoch();
    FramebufferInfo* getFramebufferInfo();
    HandoffFirmwareType getFirmwareType();

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
