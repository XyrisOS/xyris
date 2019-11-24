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

void panic_print_file(const char *file, uint32_t line);

void printPanicScreen() {
    px_tty_set_color(Black, White);
    px_clear_tty();
    px_kprint(" ________________________\n");
    px_kprint("< OH NO! Panix panicked! >\n");
    px_kprint(" ------------------------\n");
    px_kprint("        \\   ^__^\n");
    px_kprint("         \\  (XX)\\_______\n");
    px_kprint("            (__)\\       )\\/\\\n");
    px_kprint("                ||----w |\n");
    px_kprint("                ||     ||\n");
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
    px_kprint("\nEXCEPTION CAUGHT IN KERNEL MODE!\n");
    px_tty_set_color(Black, White);
    px_kprint(panicCode);
    px_kprint(px_exception_descriptions[exception]);
    // Halt the CPU
    asm("hlt");
}

void panic(char* msg, const char *file, uint32_t line) {
    // Clear the screen
    px_clear_tty();
    // Print the panic cow
    printPanicScreen();
    // Print the message passed in on a new line
    px_kprint("\n");
    px_kprint(msg);
    panic_print_file(file, line);
    // Halt the CPU
    asm("hlt");
}

void panic(registers_t regs, const char *file, uint32_t line) {
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
    px_kprint("Page fault ( ");
    if (present) { px_kprint("present "); }
    if (rw) { px_kprint("read-only "); }
    if (us) { px_kprint("user-mode "); }
    if (reserved) { px_kprint("reserved "); }
    px_kprint(") at 0x");
    px_kprint_hex(faulting_address);
    px_kprint("\n");
    panic_print_file(file, line);
    // Halt the CPU
    asm("hlt");
}

void panic_print_file(const char *file, uint32_t line) {
    char lineStr[9];    // 8 digits + 1 terminator
    px_kprint("File: ");
    px_kprint(file);
    px_kprint("\n");
    itoa(line, lineStr);
    px_kprint("Line: ");
    px_kprint(lineStr);
    px_kprint("\n");
}