/**
 * @file Arch.i386.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i386 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <arch/Memory.hpp>
#include <arch/i386/regs.hpp>
#include <arch/i386/ports.hpp>
#include <arch/i386/isr.hpp>

#define ARCH_PAGE_ALIGN 0xFFFFF000
#define ARCH_PAGE_SIZE  0x1000

namespace Arch {

namespace Memory {

inline void pageInvalidate(void* addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

inline uintptr_t pageAlign(size_t addr)
{
    return addr & ARCH_PAGE_ALIGN;
}

inline bool pageIsAligned(size_t addr)
{
    return ((addr % ARCH_PAGE_SIZE) == 0);
}

} // !namespace Arch::Memory

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};

} // !namespace Arch
