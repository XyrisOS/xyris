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

#define PAGE_ALIGN  0xFFFFF000

/*
 *    _          _      ___     _            __
 *   /_\  _ _ __| |_   |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
 *  / _ \| '_/ _| ' \   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
 * /_/ \_\_| \__|_||_| |___|_||_\__\___|_| |_| \__,_\__\___|
 */

namespace Arch {

namespace Memory {

void pagingEnable() {
    struct Registers::CR0 cr0 = Registers::readCR0();
    cr0.pagingEnable = 1;
    Registers::writeCR0(cr0);
}

void pagingDisable() {
    struct Registers::CR0 cr0 = Registers::readCR0();
    cr0.pagingEnable = 0;
    Registers::writeCR0(cr0);
}

void pageInvalidate(void* pageAddr)
{
   asm volatile("invlpg (%0)" ::"r" (pageAddr) : "memory");
}

uintptr_t pageAlign(size_t addr)
{
    return addr & PAGE_ALIGN;
}

bool pageIsAligned(size_t addr)
{
    return ((addr % ARCH_PAGE_SIZE) == 0);
}

} // !namespace Memory

namespace CPU {

void init()
{
    criticalRegion([]() {
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

const char* vendor()
{
    static int vendor[4];
    __cpuid(0, vendor[0], vendor[1], vendor[2], vendor[3]);
    return (char*)vendor;
}

const char* model()
{
    static int model[12];
    __cpuid(0x80000002, model[0], model[1], model[2], model[3]);
    __cpuid(0x80000003, model[4], model[5], model[6], model[7]);
    __cpuid(0x80000004, model[8], model[9], model[10], model[11]);
    return (char*)model;
}

} // !namespace CPU

} // !namespace Arch
