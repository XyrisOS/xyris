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

// This is a freaking massive array. It definitely makes panicking slower.
// It also eats up a significant chunk of the kernel memory since it's not
// dynamic, so maybe we need to come back to this eventually?
const char exceptionDescriptions[33][17] = {
    "Divide-By-Zero\0", "Debugging\0", "Non-Maskable\0", "Breakpoint\0",
    "Overflow\0", "Out Bound Range\0", "Invalid Opcode\0", "Device Not Avbl\0",
    "Double Fault\0", "Co-CPU Overrun\0", "Invalid TSS\0", "Sgmnt !Present\0",
    "Seg Fault\0", "Protection Flt\0", "Page Fault\0", "RESERVED\0",
    "Floating Pnt\0", "Alignment Check\0", "Machine Check\0", "SIMD Flt Pnt\0",
    "Virtualization\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "Security Excptn\0", "RESERVED\0", "Triple Fault\0", "FPU Error\0"
};

void printPanicScreen() {
    kprintSetColor(Black, White);
    clearScreen();
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
    clearScreen();
    // Print the panic cow
    printPanicScreen();
    // Get the exception code
    char* panicCode = (char*) "UNHANDLED EXCEPTION 0x00 - ";
    char* hex = (char*) "0123456789ABCDEF";
    panicCode[22] = hex[(exception >> 4) & 0xF];
    panicCode[23] = hex[exception & 0xF];
    // Print the code and associated error name
    kprint(panicCode);
    kprint(exceptionDescriptions[exception]);
    // Halt the CPU
    asm("hlt");
}

void panic(char* msg) {
    // Clear the screen
    clearScreen();
    // Print the panic cow
    printPanicScreen();
    // Print the message passed in on a new line
    kprint("\n");
    kprint(msg);
    // Halt the CPU
    asm("hlt");
}