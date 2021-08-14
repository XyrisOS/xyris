/**
 * @file Handoff.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-06-02
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
// Generic devices
#include <dev/vga/fb.hpp>

namespace Boot {

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

// TODO: Remaining information to be made obtainable
//  * PXE IP address (once we have a nice IP struct)
//  * Memory map layout (once we have new memory manager)
//  * Update Stivale2 to latest version & add missing
//  * Kernel modules (linked list of some sort?)
class Handoff {
public:
    // Constructors
    Handoff();
    Handoff(void* handoff, uint32_t magic);
    ~Handoff();
    // Getters
    const char* getCmdLine()                    { return _cmdline; }
    const void* getHandle()                     { return _handle; }
    fb::FramebufferInfo getFramebufferInfo()    { return _fbInfo; }
    HandoffBootloaderType getBootType()         { return _bootType; }

private:
    static void parseStivale2(Handoff* that, void* handoff);
    static void parseMultiboot2(Handoff* that, void* handoff);

    void* _handle;
    char* _cmdline;
    uint32_t _magic;
    fb::FramebufferInfo _fbInfo;
    HandoffBootloaderType _bootType;
};

}; // !namespace Boot
