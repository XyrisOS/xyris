/**
 * @file regs.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-12-10
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Registers {

struct CR0
{
    uint32_t protectedMode      : 1;    // Protected mode?
    uint32_t monitorCoProcessor : 1;    // Control the interaction of wait instruction?
    uint32_t emulation          : 1;    // Force all FPU operations to be emulated?
    uint32_t taskSwitched       : 1;    // Save the FPU task context?
    uint32_t extensionType      : 1;    // Is the external math coprocessor an 80287 or 80387?
    uint32_t numericError       : 1;    // FPU floating point error reporting?
    uint32_t reservedA          : 10;   // Reserved
    uint32_t writeProtection    : 1;    // Prevent the CPU from writing to read only pages?
    uint32_t reservedB          : 1;    // Reserved
    uint32_t alignmentMask      : 1;    // Automatic alignment checking?
    uint32_t reservedC          : 10;   // Reserved
    uint32_t nonWriteThrough    : 1;    // Disable write through caching?
    uint32_t cacheDisable       : 1;    // Cache disabled?
    uint32_t paging             : 1;    // Enable paging?
};

struct CR3
{
   uint32_t ignoredA            : 3;    // Ignored
   uint32_t writeThrough        : 1;    // Page level write through
   uint32_t cacheDisable        : 1;    // Cache disable
   uint32_t ignoredB            : 7;    // Ignored
   uint32_t pageDir             : 10;   // Page directory address
};

inline struct CR0 readCR0()
{
    struct CR0 x;
    asm volatile("mov %%cr0, %0": "=r"(x));
    return x;
}

inline void writeCR0(struct CR0 x)
{
    asm volatile("mov %0, %%cr0":: "r"(x));
}

inline struct CR3 readCR3()
{
    struct CR3 x;
    asm volatile("mov %%cr3, %0": "=r"(x));
    return x;
}

inline void writeCR3(struct CR3 x)
{
    asm volatile("mov %0, %%cr3":: "r"(x));
}

} // !namespace Registers
