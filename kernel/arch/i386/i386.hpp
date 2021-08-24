/**
 * @file i386.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2021-08-24
 * 
 * @copyright Copyright the Xyris Contributors (c) 2021
 * 
 */
#pragma once
#include <cpuid.h>

// Architecture inline functions
static inline void arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}
static inline int arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}
