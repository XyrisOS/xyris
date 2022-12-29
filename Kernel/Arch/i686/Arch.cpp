/**
 * @file Arch.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i686 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
// Architecture (i686) specific header
#include <Arch/i686/Arch.hpp>
#include <Arch/i686/regs.hpp>
#include <Arch/i686/gdt.hpp>
#include <Arch/i686/idt.hpp>
#include <Arch/i686/isr.hpp>
#include <Arch/i686/ports.hpp>
#include <Arch/i686/timer.hpp>
#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>
// Architecture agnostic header
#include <Arch/Arch.hpp>
#include <Arch/Memory.hpp>
#include <cpuid.h>
// Helper functions
#include <Library/stdio.hpp>
#include <Devices/Graphics/console.hpp>
#include <Devices/Serial/rs232.hpp>

const char exceptionStrings[33][32] = {
    "Divide-By-Zero", "Debugging", "Non-Maskable Interrupt", "Breakpoint",
    "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Overrun", "Invalid TSS", "Segment Not Present",
    "Segmentation Fault", "Protection Fault", "Page Fault", "RESERVED",
    "Floating Point Exception", "Alignment Check", "Machine Check", "SIMD Floating Point Exception",
    "Virtualization Exception", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "Security Exception", "RESERVED", "Triple Fault", "FPU Error",
};

/*
 *    _          _      ___     _            __
 *   /_\  _ _ __| |_   |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
 *  / _ \| '_/ _| ' \   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
 * /_/ \_\_| \__|_||_| |___|_||_\__\___|_| |_| \__,_\__\___|
 */

namespace Arch {

void registersToString(char* buf, struct registers* regs)
{
    ksprintf(
        buf,
        "\033[31m DS:\033[0m 0x%08X\n"
        "\033[31mEDI:\033[0m 0x%08X \033[31mESI:\033[0m 0x%08X \033[31mEBP:\033[0m 0x%08X \033[31mESP:\033[0m 0x%08X\n"
        "\033[31mEAX:\033[0m 0x%08X \033[31mEBX:\033[0m 0x%08X \033[31mECX:\033[0m 0x%08X \033[31mEDX:\033[0m 0x%08X\n"
        "\033[31mERR:\033[0m 0x%08X \033[31mEIP:\033[0m 0x%08X \033[31m CS:\033[0m 0x%08X\n"
        "\033[31mFLG:\033[0m 0x%08X \033[31m SS:\033[0m 0x%08X\n\n",
        regs->ds,
        regs->edi, regs->esi, regs->ebp, regs->esp,
        regs->eax, regs->ebx, regs->ecx, regs->edx,
        regs->err_code, regs->eip, regs->cs, regs->eflags,
        regs->ss);
}

void registersPrintInformation(struct registers* regs)
{
    log_all("Exception: %lu (%s)\n\n", regs->int_num, exceptionStrings[regs->int_num]);
    if (regs->err_code) {
        log_all("Error code: %lu", regs->err_code);
    }

    if (regs->int_num == Interrupts::EXCEPTION_PAGE_FAULT) {
        Registers::CR2 cr2 = Registers::readCR2();
        int missing = regs->err_code & 0x1;  // Page not present
        int rw = regs->err_code & 0x2;       // Write operation?
        int us = regs->err_code & 0x4;       // Processor was in user-mode?
        int reserved = regs->err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
        int id = regs->err_code & 0x10;      // Caused by an instruction fetch?

        const char* real = (missing ? "missing" : "present");
        const char* rws = (rw ? "reading" : "writing");
        const char* uss = (us ? "user-mode" : "kernel");
        const char* avail = (reserved ? "reserved" : "available");
        log_all("Page fault at 0x%08lX [ %s %s %s %s ] (id -> %i)\n", cr2.pageFaultAddr, real, rws, uss, avail, id);
    }
}

} // !namespace Arch

namespace Arch::Memory {

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

} // !namespace Arch::Memory

namespace Arch::CPU {

void init()
{
    criticalRegion([]() {
        GDT::init();        // Initialize the Global Descriptor Table
        Interrupts::init(); // Initialize Interrupt Service Requests
        timer_init(1000);   // Programmable Interrupt Timer (1ms)
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

} // !namespace Arch::CPU
