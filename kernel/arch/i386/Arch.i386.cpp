/**
 * @file Arch.i386.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i386 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
// Architecture (i386) specific header
#include <arch/i386/Arch.i386.hpp>
#include <arch/i386/regs.hpp>
// Architecture agnostic header
#include <arch/Arch.hpp>
#include <cpuid.h>

const char* exception_descriptions[32][16] = {
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
    Arch::criticalRegion([]() {
        gdt_install();               // Initialize the Global Descriptor Table
        isr_install();               // Initialize Interrupt Service Requests
        timer_init(1000);            // Programmable Interrupt Timer (1ms)
    });
}

void interruptsDisable() {
    asm volatile("cli");
}

void interruptsEnable() {
    asm volatile("sti");
}

void pagingEnable() {
    struct Registers::CR0 cr0 = Registers::readCR0();
    cr0.paging = 1;
    Registers::writeCR0(cr0);
}

void pagingDisable() {
    struct Registers::CR0 cr0 = Registers::readCR0();
    cr0.paging = 0;
    Registers::writeCR0(cr0);
}

void pageInvalidate(void* pageAddr)
{
   asm volatile("invlpg (%0)" ::"r" (pageAddr) : "memory");
}

const char* cpuGetVendor()
{
    static int vendor[4];
    __cpuid(0, vendor[0], vendor[1], vendor[2], vendor[3]);
    return (char*)vendor;
}

const char* cpuGetModel()
{
    static int model[12];
    __cpuid(0x80000002, model[0], model[1], model[2], model[3]);
    __cpuid(0x80000003, model[4], model[5], model[6], model[7]);
    __cpuid(0x80000004, model[8], model[9], model[10], model[11]);
    return (char*)model;
}

} // !namespace Arch
