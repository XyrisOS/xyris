/**
 * @file i386.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
// Architecture (i386) specific header
#include <arch/i386/i386.hpp>
// Architecture agnostic header
#include <arch/arch.hpp>

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


/*
 *    _          _      ___     _            __
 *   /_\  _ _ __| |_   |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
 *  / _ \| '_/ _| ' \   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
 * /_/ \_\_| \__|_||_| |___|_||_\__\___|_| |_| \__,_\__\___|
 */

namespace Arch {

void cpuInit()
{
    interrupts_disable();
    gdt_install();               // Initialize the Global Descriptor Table
    isr_install();               // Initialize Interrupt Service Requests
    timer_init(1000);            // Programmable Interrupt Timer (1ms)
    interrupts_enable();
}

void interrupts_disable() {
    asm volatile("cli");
}

void interrupts_enable() {
    asm volatile("sti");
}

const char* cpuGetVendor()
{
    static int vendor[4];
    i386::cpuid(0, vendor);
    return (char*)vendor;
}

const char* cpuGetModel()
{
    // The CPU model is broken up across 3 different calls, each using
    // EAX, EBX, ECX, and EDX to store the string, so we basically
    // are appending all 4 register values to this char array each time.
    static char model[48];
    i386::cpuid(0x80000002, (int*)(model));
    i386::cpuid(0x80000003, (int*)(model + 16));
    i386::cpuid(0x80000004, (int*)(model + 32));
    return model;
}

} // !namespace Arch
