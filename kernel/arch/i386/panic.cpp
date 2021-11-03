/**
 * @file panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A set of panic functions that are called when the kernel encounters an error from which it cannot recover.
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <arch/Arch.hpp>
#include <arch/i386/Arch.i386.hpp>
#include <arch/i386/isr.hpp>
#include <dev/graphics/console.hpp>
#include <dev/graphics/framebuffer.hpp>
#include <dev/graphics/graphics.hpp>
#include <dev/serial/rs232.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <sys/trace.hpp>

#define PANIC_COLOR_FORE 0x000000
#define PANIC_COLOR_BACK 0xFFFFFF

namespace Arch {

const char* descriptions[32][32] = {
    "Divide-By-Zero", "Debugging", "Non-Maskable Interrupt", "Breakpoint",
    "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Overrun", "Invalid TSS", "Segment Not Present",
    "Segmentation Fault", "Protection Fault", "Page Fault", "RESERVED",
    "Floating Point Exception", "Alignment Check", "Machine Check", "SIMD Floating Point Exception",
    "Virtualization Exception", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "Security Exception", "RESERVED", "Triple Fault", "FPU Error"
};

// Function prototypes
static void printPanicScreen();
static void panicPrintSource(const char* file, uint32_t line, const char* func);
static void panicPrintRegister(struct registers* regs);

static void printPanicScreen()
{
    Graphics::Framebuffer* fb = Graphics::getFramebuffer();
    Graphics::putrect(0, 0, fb->getWidth(), fb->getHeight(), PANIC_COLOR_BACK);
    Console::reset(PANIC_COLOR_FORE, PANIC_COLOR_BACK);

    char cow[256];
    ksprintf(
        cow,
        " ________________________\n"
        "%s"
        " ------------------------\n"
        "        \\   ^__^\n"
        "         \\  (XX)\\_______\n"
        "            (__)\\       )\\/\\\n"
        "             U  ||----w |\n"
        "                ||     ||\n"
        "< OH NO! Xyris panicked! >\n");
    // Print to VGA and serial
    Console::printf("%s", cow);
    RS232::printf("%s", cow);
}

NORET void panic(const char* msg, const char* file, uint32_t line, const char* func)
{
    asm volatile("cli");
    // Print the panic cow
    printPanicScreen();
    // Print to VGA and serial
    Console::printf("\n%s\n", msg);
    RS232::printf("\n%s\n", msg);
    // Print out file info to describe where crash occured
    panicPrintSource(file, line, func);
    stack_trace(16);
    // Halt the CPU
    while (true) {
        asm("hlt");
    }
}

NORET void panic(struct registers* regs, const char* file, uint32_t line, const char* func)
{
    asm volatile("cli");
    // Print the panic cow and exception description
    printPanicScreen();
    char msg[128];
    ksprintf(
        msg,
        "Exception: %i (%s)\n\n",
        regs->int_num,
        descriptions[regs->int_num]);
    // Print to VGA and serial
    Console::printf("%s", msg);
    RS232::printf("%s", msg);
    // Check if we have an error code and print
    if (regs->err_code) {
        ksprintf(
            msg,
            "Error code: %i",
            regs->err_code);
        // Print to VGA and serial
        Console::printf("%s", msg);
        RS232::printf("%s", msg);
    }
    // Print out register values
    panicPrintRegister(regs);
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    Registers::CR2 cr2 = Registers::readCR2();
    // The error code gives us details of what happened.
    int missing = regs->err_code & 0x1;    // Page not present
    int rw = regs->err_code & 0x2;         // Write operation?
    int us = regs->err_code & 0x4;         // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;   // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;        // Caused by an instruction fetch?
    // If we have a page fault, print out page fault info
    if (regs->int_num == ISR_PAGE_FAULT) {
        // Output an error message.
        const char* real = (missing ? "missing " : "present ");
        const char* rws = (rw ? "reading " : "writing ");
        const char* uss = (us ? "user-mode " : "kernel ");
        const char* avail = (reserved ? "reserved" : "available");
        // Now that we assigned all of our string, put together the message
        ksprintf(
            msg,
            "Page fault (%s%s%s%s) at 0x%08X (id -> %i)\n",
            real,
            rws,
            uss,
            avail,
            cr2.pageFaultAddr,
            id);
        // Print to VGA and serial
        Console::printf("%s", msg);
        RS232::printf("%s", msg);
    }
    panicPrintSource(file, line, func);
    stack_trace(16);
    // Halt the CPU
    while (true) {
        asm("hlt");
    }
}

static void panicPrintSource(const char* file, uint32_t line, const char* func)
{
    char msg[128];
    ksprintf(
        msg,
        "Crash location may be inaccurate.\n"
        "File: %s\nFunc: %s\nLine: %i\n",
        file,
        func,
        line);
    // Print to VGA and serial
    Console::printf("%s", msg);
    RS232::printf("%s", msg);
}

static void panicPrintRegister(struct registers* regs)
{
    char msg[512];
    ksprintf(
        msg,
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
    // Print to VGA and serial
    Console::printf("%s", msg);
    RS232::printf("%s", msg);
}

}
