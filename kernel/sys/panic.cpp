/**
 * @file panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A set of panic functions that are called when the kernel
 * encounters an error from which it cannot recover.
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#include <sys/panic.hpp>
#include <devices/tty/tty.hpp>
#include <devices/serial/rs232.hpp>
#include <lib/string.hpp>
#include <lib/stdio.hpp>

// Function prototypes
void panic_print_file(const char *file, uint32_t line, const char *func);
void panic_print_register(registers_t *regs);

void printPanicScreen(int exception) {
    //px_tty_set_color(VGA_Black, VGA_White);
    px_clear_tty(VGA_Black, VGA_White);
    char* tag;
    if (exception == 13) {
        tag = "< Wait... That's Illegal >\n";
    } else {
        tag = "< OH NO! Panix panicked! >\n";
    }
    char cow[256];
    px_ksprintf(
        cow,
        " ________________________\n"
        "%s"
        " ------------------------\n"
        "        \\   ^__^\n"
        "         \\  (XX)\\_______\n"
        "            (__)\\       )\\/\\\n"
        "             U  ||----w |\n"
        "                ||     ||\n",
        tag
    );
    // Print to VGA and serial
    px_kprintf("%s", cow);
    px_rs232_print(cow);
}

void panic(char* msg, const char *file, uint32_t line, const char *func) {
    // Print the panic cow
    printPanicScreen(0);
    // Print the message passed in on a new line
    char buf[128];
    px_ksprintf(buf, "\n%s\n", msg);
    // Print to VGA and serial
    px_kprintf("%s", buf);
    px_rs232_print(buf);
    // Print out file info to describe where crash occured
    panic_print_file(file, line, func);
    // Halt the CPU
    asm("cli");
    asm("hlt");
}

void panic(registers_t *regs, const char *file, uint32_t line, const char *func) {
    // Print the panic cow and exception description
    printPanicScreen(regs->int_num);
    char msg[64];
    px_ksprintf(
        msg, 
        "Exception: %i (%s)\n\n",
        regs->int_num,
        px_exception_descriptions[regs->int_num]
    );
    // Print to VGA and serial
    px_kprintf("%s", msg);
    px_rs232_print(msg);
    // Check if we have an error code and print
    if (regs->err_code) {
        px_ksprintf(
            msg,
            "Error code: %i",
            regs->err_code
        );
        // Print to VGA and serial
        px_kprintf("%s", msg);
        px_rs232_print(msg);
    }
    // Print out register values
    panic_print_register(regs);
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    // The error code gives us details of what happened.
    int present = !(regs->err_code & 0x1);   // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?
    // If we have a page fault, print out page fault info
    if (regs->int_num == 14) {
        // Output an error message.
        const char* real = (present ? "present " : "missing ");
        const char* rws = (rw ? "reading " : "writing ");
        const char* uss = (us ? "user-mode " : "kernel ");
        const char* avail = (reserved ? "reserved" : "available");
        // Now that we assigned all of our string, put together the message
        char msg[128];
        px_ksprintf(
            msg,
            "Page fault (%s%s%s%s) at 0x0x%08X\n",
            real,
            rws,
            uss,
            avail,
            faulting_address
        );
        // Print to VGA and serial
        px_kprintf("%s", msg);
        px_rs232_print(msg);
    }
    panic_print_file(file, line, func);
    // Halt the CPU
    asm("cli");
    asm("hlt");
}

void panic_print_file(const char *file, uint32_t line, const char *func) {
    char msg[128];
    px_ksprintf(
        msg,
        "Crash location may be inaccurate.\n"
        "File: %s\nFunc: %s\nLine: %i\n",
        file,
        func,
        line
    );
    // Print to VGA and serial
    px_kprintf("%s", msg);
    px_rs232_print(msg);
}

void panic_print_register(registers_t *regs) {
    // I really wanted to add color codes here to make the register labels
    // red, but that would also mean resetting the background *and* foreground
    // colors each time (to print the numbers in back) since I can't just call
    // reset (because it would reset to a black background w/ white text, which
    // is the inverse of what we want.)
    char msg[256];
    px_ksprintf(
        msg,
        " DS: 0x%08X\n"
        "EDI: 0x%08X ESI: 0x%08X EBP: 0x%08X ESP: 0x%08X\n"
        "EAX: 0x%08X EBX: 0x%08X ECX: 0x%08X EDX: 0x%08X\n"
        "ERR: 0x%08X EIP: 0x%08X  CS: 0x%08X\n"
        "FLG: 0x%08X  SS: 0x%08X\n\n",
        regs->ds,
        regs->edi, regs->esi, regs->ebp, regs->esp,
        regs->eax, regs->ebx, regs->ecx, regs->edx,
        regs->err_code, regs->eip, regs->cs, regs->eflags,
        regs->ss
    );
    // Print to VGA and serial
    px_kprintf("%s", msg);
    px_rs232_print(msg);
}