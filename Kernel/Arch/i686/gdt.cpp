/**
 * @file gdt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The Global Descriptor Table (GDT) is specific to the IA32 architecture.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <Arch/i686/gdt.hpp>
#include <Arch/i686/Assembly/Flush.h>
#include <Library/string.hpp>

#define ARCH_GDT_MAX_ENTRIES 5

namespace GDT {

struct Entry gdt[ARCH_GDT_MAX_ENTRIES];
struct Registers::GDTR gdtr;

void init()
{
    uint8_t gdtIndex = 0;
    const union Base nullSegmentBase = { .value = 0 };
    const union Limit nullSegmentLimit = { .value = 0 };
    gdt[gdtIndex++] = {
        .limit_low = nullSegmentLimit.section.low,
        .base_low = nullSegmentBase.section.low,
        .accessed = 0,
        .rw = 0,
        .dc = 0,
        .executable = 0,
        .system = 0,
        .privilege = 0,
        .present = 0,
        .limit_high = nullSegmentLimit.section.high,
        .reserved = 0,
        .longMode = 0,
        .size = 0,
        .granulatity = 0,
        .base_high = nullSegmentBase.section.high,
    };

    const union Base kernelCodeBase = { .value = 0 };
    const union Limit kernelCodeLimit = { .value = 0x000FFFFF };
    gdt[gdtIndex++] = {
        .limit_low = kernelCodeLimit.section.low,
        .base_low = kernelCodeBase.section.low,
        .accessed = 0,
        .rw = 1,
        .dc = 0,
        .executable = 1,
        .system = 1,
        .privilege = 0,
        .present = 1,
        .limit_high = kernelCodeLimit.section.high,
        .reserved = 0,
        .longMode = 0,
        .size = 1,
        .granulatity = 1,
        .base_high = kernelCodeBase.section.high,
    };

    const union Base kernelDataBase = { .value = 0 };
    const union Limit kernelDataLimit = { .value = 0x000FFFFF };
    gdt[gdtIndex++] = {
        .limit_low = kernelDataLimit.section.low,
        .base_low = kernelDataBase.section.low,
        .accessed = 0,
        .rw = 1,
        .dc = 0,
        .executable = 0,
        .system = 1,
        .privilege = 0,
        .present = 1,
        .limit_high = kernelDataLimit.section.high,
        .reserved = 0,
        .longMode = 0,
        .size = 1,
        .granulatity = 1,
        .base_high = kernelDataBase.section.high,
    };

    const union Base userCodeBase = { .value = 0 };
    const union Limit userCodeLimit = { .value = 0x000FFFFF };
    gdt[gdtIndex++] = {
        .limit_low = userCodeLimit.section.low,
        .base_low = userCodeBase.section.low,
        .accessed = 0,
        .rw = 1,
        .dc = 0,
        .executable = 1,
        .system = 1,
        .privilege = 3,
        .present = 1,
        .limit_high = userCodeLimit.section.high,
        .reserved = 0,
        .longMode = 0,
        .size = 1,
        .granulatity = 1,
        .base_high = userCodeBase.section.high,
    };

    const union Base userDataBase = { .value = 0 };
    const union Limit userDataLimit = { .value = 0x000FFFFF };
    gdt[gdtIndex++] = {
        .limit_low = userDataLimit.section.low,
        .base_low = userDataBase.section.low,
        .accessed = 0,
        .rw = 1,
        .dc = 0,
        .executable = 0,
        .system = 1,
        .privilege = 3,
        .present = 1,
        .limit_high = userDataLimit.section.high,
        .reserved = 0,
        .longMode = 0,
        .size = 1,
        .granulatity = 1,
        .base_high = userDataBase.section.high,
    };

    // Update GDT register and flush
    gdtr.size = sizeof(gdt) - 1;
    gdtr.base = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gdtr);
}

} // !namespace GDT
