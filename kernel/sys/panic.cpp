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
    kprint(px_exception_descriptions[exception]);
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