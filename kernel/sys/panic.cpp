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
    px_kprintf("");
    if (exception == 13) {
        tag = "< Wait... That's Illegal >\n";
    } else {
        tag = "< OH NO! Panix panicked! >\n";
    }
    char cow[256];
    px_ksprintf(cow,
        " ________________________\n"
        "%s"
        " ------------------------\n"
        "        \\   ^__^\n"
        "         \\  (XX)\\_______\n"
        "            (__)\\       )\\/\\\n"
        "                ||----w |\n"
        "                ||     ||\n",
        tag
    );
    px_kprintf("%s", cow);
    px_rs232_print(cow);
}

void panic(int exception) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen(exception);
    // Get the exception code
    char* panicCode = (char*) "UNHANDLED EXCEPTION 0x00 - ";
    char* hex = (char*) "0123456789ABCDEF";
    panicCode[22] = hex[(exception >> 4) & 0xF];
    panicCode[23] = hex[exception & 0xF];
    // Print the code and associated error name
    px_kprintf("\n\033[91mEXCEPTION CAUGHT IN KERNEL MODE!\n");
    px_kprintf(panicCode);
    px_kprintf(px_exception_descriptions[exception]);
    // Halt the CPU
    asm("hlt");
}

void panic(char* msg, const char *file, uint32_t line, const char *func) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen(0);
    // Print the message passed in on a new line
    px_kprintf("\n%s", msg);
    panic_print_file(file, line, func);
    // Halt the CPU
    asm("hlt");
}

void panic(registers_t *regs, const char *file, uint32_t line, const char *func) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow and exception description
    printPanicScreen(regs->int_num);
    px_kprintf("Exception: %i (%s)", regs->int_num, px_exception_descriptions[regs->int_num]);
    if (regs->err_code) {
        px_kprintf("Error code: %i", regs->err_code);
    }
    px_kprintf("\n\n");
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
        px_kprintf("Page fault ( ");
        (present) ? px_kprintf("present ") : px_kprintf("missing ");
        (rw) ? px_kprintf("reading ") : px_kprintf("writing ");
        (us) ? px_kprintf("user-mode ") : px_kprintf("kernel ");
        (reserved) ? px_kprintf("reserved ") : px_kprintf("available");
        px_kprintf(") at 0x0x%08X", faulting_address);
    }
    px_kprintf("\n");
    panic_print_file(file, line, func);
    // Halt the CPU
    asm("hlt");
}

void panic_print_file(const char *file, uint32_t line, const char *func) {
    px_kprintf("File: %s\nFunc: %s\nLine: %i\n", file, func, line);
}

void panic_print_register(registers_t *regs) {
    // I really wanted to add color codes here to make the register labels
    // red, but that would also mean resetting the background *and* foreground
    // colors each time (to print the numbers in back) since I can't just call
    // reset (because it would reset to a black background w/ white text, which
    // is the inverse of what we want.)
    px_kprintf(
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
}