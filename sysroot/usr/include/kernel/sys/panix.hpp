/**
 * @file panix.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Includes many commonly used system functions.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#ifndef PANIX_SYS_HPP
#define PANIX_SYS_HPP

#include <sys/types.hpp>            // Data type definitions
#include <arch/arch.hpp>            // Architecture specific features
#include <devices/tty/tty.hpp>   // Necessary for printing to the TTY

// Macros for panic and assert
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)
#define assert(x) (!(x) ? (PANIC("Assert failed at ")) : (void)0)

// List of all exceptions and their associated english descriptions
extern const char* px_exception_descriptions[];

// Kernel panic functions
void panic(int exception);
void panic(char* msg, const char *file, uint32_t line, const char *func);
void panic(registers_t *regs, const char *file, uint32_t line, const char *func);

// Userspace
extern "C" void jump_usermode(uintptr_t location, uintptr_t stack);

#endif /* PANIX_SYS_HPP */