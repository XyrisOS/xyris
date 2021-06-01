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
#include <lib/stdio.hpp>

/* Shared i386 & amd64 */
#if defined(__i386__) | defined(__i686__) | defined(__amd64__) | defined(__x86_64__)

const char* exception_descriptions[] = {
    "Divide-By-Zero", "Debugging", "Non-Maskable", "Breakpoint",
    "Overflow", "Out Bound Range", "Invalid Opcode", "Device Not Avbl",
    "Double Fault", "Co-CPU Overrun", "Invalid TSS", "Sgmnt !Present",
    "Seg Fault", "Protection Flt", "Page Fault", "RESERVED",
    "Floating Pnt", "Alignment Check", "Machine Check", "SIMD Flt Pnt",
    "Virtualization", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "Security Excptn", "RESERVED", "Triple Fault", "FPU Error"
};

const char* cpu_get_vendor() {
    static int vendor[4];
    __asm__ volatile ("cpuid" : "=a"(vendor[0]), "=b"(vendor[0]), "=d"(vendor[1]), "=c"(vendor[2]) : "a"(0));
    return (char*)vendor;
}

const char* cpu_get_model() {
    // The CPU model is broken up across 3 different calls, each using
    // EAX, EBX, ECX, and EDX to store the string, so we basically
    // are appending all 4 register values to this char array each time.
    static char model[48];
    arch_cpuid(0x80000002, (int *)(model));
    arch_cpuid(0x80000003, (int *)(model+16));
    arch_cpuid(0x80000004, (int *)(model+32));
    return model;
}

#endif /* End shared i386 & amd64 */
