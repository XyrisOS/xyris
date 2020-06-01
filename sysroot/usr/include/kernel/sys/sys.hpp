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

#include <sys/types.hpp>            // Every file needs to know the available data types
#include <arch/x86/ports.hpp>       // We should basically always have access to port functions
#include <devices/tty/kprint.hpp>   // Printing is a pretty common need, so we'll include it
#include <mem/heap.hpp>
// Macros for panic and assert
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)
#define assert(x) (!(x) ? (PANIC("Assert failed at ")) : (void)0)

// List of all exceptions and their associated english descriptions
extern const char* px_exception_descriptions[];

// Kernel utility functions
void panic(int exception);
void panic(char* msg, const char *file, uint32_t line, const char *func);
void panic(registers_t regs, const char *file, uint32_t line, const char *func);
// String functions
int strlen(const char* s);
void strcpy(const char* source, char* destination);
char* strcat(const char *s1, const char *s2);
char* to_upper(char* string);
void reverse(char* s);
void itoa(int n, char str[]);
// Memory functions
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);
// Userspace
extern "C" void jump_usermode(uintptr_t location, uintptr_t stack);;

#endif /* PANIX_SYS_HPP */