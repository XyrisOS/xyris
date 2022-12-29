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
#include <Devices/Graphics/framebuffer.hpp>
#include <Memory/MemoryMap.hpp>
#include <stdint.h>

namespace Boot {

enum HandoffBootloaderType {
    Stivale2 = 0,
};

// Unused for now.
class HandoffRSDPDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffRSDPDescriptor();
    ~HandoffRSDPDescriptor();
    // Getters
    const char* getSignature()          { return m_signature; }
    uint8_t getChecksum()               { return m_checksum; }
    const char* getOEMID()              { return m_OEMID; }
    uint8_t getRevision()               { return m_revision; }
    uint32_t getRSDTAddress()           { return m_rsdtAddress; }

private:
    char m_signature[8];
    uint8_t m_checksum;
    char m_OEMID[6];
    uint8_t m_revision;
    uint32_t m_rsdtAddress;
};

// Unused for now.
class HandoffCPUDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffCPUDescriptor();
    ~HandoffCPUDescriptor();
    // Getters
    uint32_t getCPUID()                 { return m_cpuID; }
    uint32_t getLAPICID()               { return m_lapicID; }
    void* getStackAddr()                { return m_stackAddr; }
    void* getGotoAddr()                 { return m_gotoAddr; }
    void* getArguments()                { return m_arguments; }

private:
    uint32_t m_cpuID;
    uint32_t m_lapicID;
    void* m_stackAddr;
    void* m_gotoAddr;
    void* m_arguments;
};

// Unused for now.
class HandoffSMPDescriptor {
public:
    // Constructors
    // TODO: Parameters
    HandoffSMPDescriptor();
    ~HandoffSMPDescriptor();
    // Getters
    bool getX2Available()               { return m_x2Available; }
    uint32_t getLAPICID()               { return m_lapicID; }
    uint64_t getCPUCount()              { return m_cpuCount; }

private:
    bool m_x2Available;
    uint32_t m_lapicID;
    uint64_t m_cpuCount;
};

// TODO: Remaining information to be made obtainable
//  * PXE IP address (once we have a nice IP struct)
//  * Update Stivale2 to latest version & add missing
//  * Kernel modules (linked list of some sort?)
class Handoff {
public:
    // Constructors
    Handoff();
    Handoff(void* handoff, uint32_t magic);
    // Getters
    const char* CmdLine()                       { return m_cmdline; }
    const void* Handle()                        { return m_handle; }
    Graphics::Framebuffer* FramebufferInfo()    { return &m_framebuffer; }
    HandoffBootloaderType* BootType()           { return &m_bootType; }
    Memory::MemoryMap& MemoryMap()              { return m_memoryMap; }

private:
    static void parseStivale2(Handoff* that, void* handoff);

    void* m_handle;
    char* m_cmdline;
    uint32_t m_magic;
    Graphics::Framebuffer m_framebuffer;
    HandoffBootloaderType m_bootType;
    Memory::MemoryMap m_memoryMap;
};

}; // !namespace Boot
