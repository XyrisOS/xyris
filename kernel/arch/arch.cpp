/**
 * @file arch.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-06-01
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#include <arch/arch.hpp>

const char* px_exception_descriptions[] = {
    "Divide-By-Zero\0", "Debugging\0", "Non-Maskable\0", "Breakpoint\0",
    "Overflow\0", "Out Bound Range\0", "Invalid Opcode\0", "Device Not Avbl\0",
    "Double Fault\0", "Co-CPU Overrun\0", "Invalid TSS\0", "Sgmnt !Present\0",
    "Seg Fault\0", "Protection Flt\0", "Page Fault\0", "RESERVED\0",
    "Floating Pnt\0", "Alignment Check\0", "Machine Check\0", "SIMD Flt Pnt\0",
    "Virtualization\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "Security Excptn\0", "RESERVED\0", "Triple Fault\0", "FPU Error\0"
};

/**
 * @brief 
 * 
 * @param code 
 * @param str 
 * @return int 
 */
static inline void px_arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}

static inline int px_arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}

static inline int px_arch_cpuid_vendor(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+0)), "=d"(*(regs+1)), "=c"(*(regs+2)) : "a"(flag));
    return (int)regs[0];
}

const char* const px_cpu_get_vendor() {
	static char vendor[16];
	px_arch_cpuid_vendor(0, (int *)(vendor));
	return vendor;
}

const char* const px_cpu_get_model() {
    // The CPU model is broken up across 3 different calls, each using
    // EAX, EBX, ECX, and EDX to store the string, so we basically
    // are appending all 4 register values to this char array each time.
    static char model[48];
	px_arch_cpuid(0x80000002, (int *)(model));
    px_arch_cpuid(0x80000003, (int *)(model+16));
    px_arch_cpuid(0x80000004, (int *)(model+32));
	return model;
}