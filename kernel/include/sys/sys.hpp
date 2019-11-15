/**
 * @file sys.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Includes many commonly used system functions.
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_SYS_HPP
#define PANIX_SYS_HPP

#include <sys/types.hpp>
#include <devices/tty/kprint.hpp>
// List of all exceptions and their associated english descriptions
const char px_exception_descriptions[33][17] = {
    "Divide-By-Zero\0", "Debugging\0", "Non-Maskable\0", "Breakpoint\0",
    "Overflow\0", "Out Bound Range\0", "Invalid Opcode\0", "Device Not Avbl\0",
    "Double Fault\0", "Co-CPU Overrun\0", "Invalid TSS\0", "Sgmnt !Present\0",
    "Seg Fault\0", "Protection Flt\0", "Page Fault\0", "RESERVED\0",
    "Floating Pnt\0", "Alignment Check\0", "Machine Check\0", "SIMD Flt Pnt\0",
    "Virtualization\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "Security Excptn\0", "RESERVED\0", "Triple Fault\0", "FPU Error\0"
};

// Kernel utility functions
void panic(int exception);
void panic(char* msg);
// String functions
int strlen(const char* s);
char* concat(const char *s1, const char *s2);
void itoa(int n, char str[]);
// Memory functions
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);
// Userspace
extern "C" void jump_usermode(uintptr_t location, uintptr_t stack);;

#endif /* PANIX_SYS_HPP */