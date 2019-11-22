/**
 * @file panic.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A set of panic functions that are called when the kernel
 * encounters an error from which it cannot recover.
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */
#include <sys/sys.hpp>

void printPanicScreen() {
    px_tty_set_color(Black, White);
    px_clear_tty();
    kprint(" ________________________\n");
    kprint("< OH NO! Panix panicked! >\n");
    kprint(" ------------------------\n");
    kprint("        \\   ^__^\n");
    kprint("         \\  (XX)\\_______\n");
    kprint("            (__)\\       )\\/\\\n");
    kprint("                ||----w |\n");
    kprint("                ||     ||\n");
}

void panic(int exception) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen();
    // Get the exception code
    char* panicCode = (char*) "UNHANDLED EXCEPTION 0x00 - ";
    char* hex = (char*) "0123456789ABCDEF";
    panicCode[22] = hex[(exception >> 4) & 0xF];
    panicCode[23] = hex[exception & 0xF];
    // Print the code and associated error name
    px_tty_set_color(Red, White);
    kprint("\nEXCEPTION CAUGHT IN KERNEL MODE!\n");
    px_tty_set_color(Black, White);
    kprint(panicCode);
    kprint(px_exception_descriptions[exception]);
    // Halt the CPU
    asm("hlt");
}

void panic(char* msg) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen();
    // Print the message passed in on a new line
    kprint("\n");
    kprint(msg);
    // Halt the CPU
    asm("hlt");
}

void panic(registers_t regs) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen();
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
 
    // The error code gives us details of what happened.
    int present = !(regs.err_code & 0x1);   // Page not present
    int rw = regs.err_code & 0x2;           // Write operation?
    int us = regs.err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs.err_code & 0x10;          // Caused by an instruction fetch?
 
    // Output an error message.
    kprint("Page fault! ( ");
    if (present) { kprint("present "); }
    if (rw) { kprint("read-only "); }
    if (us) { kprint("user-mode "); }
    if (reserved) { kprint("reserved "); }
    kprint(") at 0x");
    kprint_hex(faulting_address);
    kprint("\n");
}