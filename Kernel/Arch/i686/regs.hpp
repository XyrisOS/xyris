/**
 * @file regs.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i686 control register definitions. C & C++ compatible header.
 * @version 0.3
 * @date 2019-12-10
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * @brief A structure definining values for all x86 registers.
 * Cannot be namespaced due to C linkage and ASM interoperability
 */
struct registers {
    uint32_t ds;                                         /* Data segment selector */
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax; /* Pushed by pusha. */
    uint32_t int_num, err_code;                          /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, esp, ss;                   /* Pushed by the processor automatically */
} __attribute__((packed));

#ifdef __cplusplus
namespace Registers {
#endif

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
    uint32_t pagingEnable       : 1;    // Enable paging
} __attribute__((packed));

struct CR2
{
    uint32_t pageFaultAddr      : 32;   // Address where page fault occured
} __attribute__((packed));

struct CR3
{
   uint32_t ignoredA            : 3;    // Ignored
   uint32_t writeThrough        : 1;    // Page level write through
   uint32_t cacheDisable        : 1;    // Cache disable
   uint32_t ignoredB            : 7;    // Ignored
   uint32_t pageDir             : 20;   // Page directory physical address
} __attribute__((packed));

// A pointer to the array of interrupt handlers. Assembly instruction 'lidt' will read it
struct IDTR {
    uint16_t size   : 16;
    uint32_t base   : 32;
} __attribute__((packed));

// A pointer to the global descriptor table. Assembly 'lgdt' will read it.
struct GDTR {
    uint16_t size   : 16;
    uint32_t base   : 32;
} __attribute__((packed));

__attribute__((always_inline))
static inline struct CR0 readCR0(void)
{
    struct CR0 x;
    asm volatile("mov %%cr0, %0": "=r"(x));
    return x;
}

__attribute__((always_inline))
static inline void writeCR0(struct CR0 x)
{
    asm volatile("mov %0, %%cr0":: "r"(x));
}

static inline struct CR2 readCR2(void)
{
    struct CR2 x;
    asm volatile("mov %%cr2, %0" : "=r"(x));
    return x;
}

static inline struct CR3 readCR3(void)
{
    struct CR3 x;
    asm volatile("mov %%cr3, %0": "=r"(x));
    return x;
}

static inline void writeCR3(struct CR3 x)
{
    asm volatile("mov %0, %%cr3":: "r"(x));
}

#ifdef __cplusplus
} // !namespace Registers
#endif
