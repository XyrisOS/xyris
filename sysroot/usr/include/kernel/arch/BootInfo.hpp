#pragma once
#include <mem/MemoryRange.hpp>

#define MAX_MEMORY_RANGES   64
#define MAX_KERNEL_MODULES  64

struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

struct BootInfo
{
    size_t magic;
    void* payload;

    size_t memoryUsable;

    Memory::MemoryRange memoryMap[MAX_MEMORY_RANGES];
    size_t memoryMapSize;

    uintptr_t fbAddr;
    size_t fbWidth;
    size_t fbHeight;
    size_t fbPitch;
    size_t fbBPP;

    RSDPDescriptor acpiRsdp;
};

void px_parse_multiboot2(BootInfo* handle, void* info);
void px_parse_stivale2(BootInfo* handle, void* info);
